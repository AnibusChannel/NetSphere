"""
Генератор статической HTML-документации по docstring'ам Python-кода.

Подход:
- не импортирует модули (важно для GUI-кода, который зависит от PyQt);
- парсит AST (`ast.parse`) и извлекает docstring'и модулей, классов и функций;
- строит простые HTML-страницы без внешних зависимостей.

Использование:
    python generate_html_docs.py
"""

from __future__ import annotations

import argparse
import ast
import html
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional


@dataclass
class FunctionDoc:
    name: str
    signature: str
    docstring: str = ""


@dataclass
class ClassDoc:
    name: str
    docstring: str = ""
    methods: List[FunctionDoc] = field(default_factory=list)


@dataclass
class ModuleDoc:
    name: str
    docstring: str = ""
    classes: List[ClassDoc] = field(default_factory=list)
    functions: List[FunctionDoc] = field(default_factory=list)


def _unparse(node: ast.AST) -> str:
    """Бережно приводит AST-узел к строке (через ast.unparse при наличии)."""
    try:
        return ast.unparse(node)
    except Exception:
        return ""


def _format_args(args: ast.arguments) -> str:
    """
    Формирует строку аргументов вида: "a, b=1, *args, **kwargs".
    Типы аннотаций добавляются как часть строки сигнатуры.
    """

    def fmt_arg(arg: ast.arg) -> str:
        if arg.annotation is None:
            return arg.arg
        return f"{arg.arg}: {_unparse(arg.annotation)}"

    parts: List[str] = []
    normal = args.posonlyargs + args.args
    defaults = list(args.defaults)

    # Align defaults with the last N normal args.
    num_no_defaults = len(normal) - len(defaults)
    for i, a in enumerate(normal):
        if i < num_no_defaults:
            parts.append(fmt_arg(a))
        else:
            d = defaults[i - num_no_defaults]
            parts.append(f"{fmt_arg(a)}={_unparse(d)}")

    if args.vararg is not None:
        if args.vararg.annotation is None:
            parts.append(f"*{args.vararg.arg}")
        else:
            parts.append(f"*{args.vararg.arg}: {_unparse(args.vararg.annotation)}")

    if args.kwonlyargs:
        for a in args.kwonlyargs:
            # kw-only defaults are handled by kw_defaults alignment
            # - if default is None => no default
            pass
        for a, d in zip(args.kwonlyargs, args.kw_defaults):
            if d is None:
                parts.append(fmt_arg(a))
            else:
                parts.append(f"{fmt_arg(a)}={_unparse(d)}")

    if args.kwarg is not None:
        if args.kwarg.annotation is None:
            parts.append(f"**{args.kwarg.arg}")
        else:
            parts.append(f"**{args.kwarg.arg}: {_unparse(args.kwarg.annotation)}")

    return ", ".join([p for p in parts if p])


def _function_signature(node: ast.FunctionDef | ast.AsyncFunctionDef, *, drop_self: bool = True) -> str:
    """Строит подпись функции по AST (пример: 'foo(a: int, b=1) -> str')."""
    args = node.args
    arg_list = args.posonlyargs + args.args

    if drop_self and arg_list:
        first_name = arg_list[0].arg
        if first_name in {"self", "cls"}:
            # Create a shallow copy with removed first arg (for formatting only).
            new_args = ast.arguments(
                posonlyargs=args.posonlyargs,
                args=args.args[1:],
                vararg=args.vararg,
                kwonlyargs=args.kwonlyargs,
                kw_defaults=args.kw_defaults,
                kwarg=args.kwarg,
                defaults=args.defaults[1:] if args.defaults else [],
            )
            args_str = _format_args(new_args)
        else:
            args_str = _format_args(args)
    else:
        args_str = _format_args(args)

    ret = f" -> {_unparse(node.returns)}" if node.returns is not None else ""
    return f"{node.name}({args_str}){ret}"


def parse_module(path: Path) -> ModuleDoc:
    """Парсит один .py файл и извлекает docstring'и модулей/классов/функций."""
    tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))

    mod_doc = ModuleDoc(name=path.stem, docstring=ast.get_docstring(tree) or "")

    for node in tree.body:
        if isinstance(node, ast.ClassDef):
            cls = ClassDoc(name=node.name, docstring=ast.get_docstring(node) or "")
            for item in node.body:
                if isinstance(item, (ast.FunctionDef, ast.AsyncFunctionDef)):
                    cls.methods.append(
                        FunctionDoc(
                            name=item.name,
                            signature=_function_signature(item),
                            docstring=ast.get_docstring(item) or "",
                        )
                    )
            mod_doc.classes.append(cls)
        elif isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
            mod_doc.functions.append(
                FunctionDoc(
                    name=node.name,
                    signature=_function_signature(node, drop_self=False),
                    docstring=ast.get_docstring(node) or "",
                )
            )

    return mod_doc


def render_module_html(mod: ModuleDoc, output_dir: Path) -> None:
    """Рендерит HTML-страницу для одного модуля."""
    parts: List[str] = []
    parts.append("<!doctype html>")
    parts.append("<html lang='ru'>")
    parts.append("<head>")
    parts.append("<meta charset='utf-8'/>")
    parts.append(f"<title>{html.escape(mod.name)}</title>")
    parts.append(
        """
        <style>
          body { font-family: Arial, Helvetica, sans-serif; margin: 24px; line-height: 1.35; }
          code, pre { background: #f5f5f5; padding: 2px 4px; border-radius: 4px; }
          pre { white-space: pre-wrap; }
          h1 { margin-top: 0; }
          .doc { margin: 8px 0 18px 0; }
          .section { margin: 18px 0; padding: 12px; border: 1px solid #eee; border-radius: 8px; }
          .sig { font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, 'Liberation Mono', 'Courier New', monospace; }
        </style>
        """
    )
    parts.append("</head>")
    parts.append("<body>")

    parts.append(f"<h1>Модуль: {html.escape(mod.name)}</h1>")
    if mod.docstring:
        parts.append(f"<div class='doc'>{html.escape(mod.docstring).replace('\\n', '<br/>')}</div>")

    if mod.functions:
        parts.append("<div class='section'><h2>Функции</h2>")
        for fn in mod.functions:
            parts.append("<div class='section'>")
            parts.append(f"<div class='sig'><strong>{html.escape(fn.signature)}</strong></div>")
            if fn.docstring:
                parts.append(
                    "<div class='doc'>"
                    + html.escape(fn.docstring).replace("\n", "<br/>")
                    + "</div>"
                )
            parts.append("</div>")
        parts.append("</div>")

    if mod.classes:
        parts.append("<div class='section'><h2>Классы</h2>")
        for cls in mod.classes:
            parts.append("<div class='section'>")
            parts.append(f"<h3>{html.escape(cls.name)}</h3>")
            if cls.docstring:
                parts.append(
                    "<div class='doc'>"
                    + html.escape(cls.docstring).replace("\n", "<br/>")
                    + "</div>"
                )
            if cls.methods:
                parts.append("<h4>Методы</h4>")
                for m in cls.methods:
                    parts.append("<div class='section'>")
                    parts.append(f"<div class='sig'><strong>{html.escape(m.signature)}</strong></div>")
                    if m.docstring:
                        parts.append(
                            "<div class='doc'>"
                            + html.escape(m.docstring).replace("\n", "<br/>")
                            + "</div>"
                        )
                    parts.append("</div>")
            parts.append("</div>")
        parts.append("</div>")

    parts.append("<p><hr/><a href='index.html'>К оглавлению</a></p>")
    parts.append("</body></html>")

    out_path = output_dir / f"{mod.name}.html"
    out_path.write_text("\n".join(parts), encoding="utf-8")


def render_index_html(modules: List[ModuleDoc], output_dir: Path) -> None:
    parts: List[str] = []
    parts.append("<!doctype html><html lang='ru'><head><meta charset='utf-8'/>")
    parts.append(f"<title>Python docs (docstrings)</title>")
    parts.append(
        """
        <style>
          body { font-family: Arial, Helvetica, sans-serif; margin: 24px; line-height: 1.35; }
          a { color: #0645ad; text-decoration: none; }
          a:hover { text-decoration: underline; }
          .mod { margin: 10px 0; padding: 10px; border: 1px solid #eee; border-radius: 8px; }
          code { background: #f5f5f5; padding: 2px 4px; border-radius: 4px; }
        </style>
        """
    )
    parts.append("</head><body>")
    parts.append("<h1>Документация по Python-модулям (docstrings)</h1>")
    parts.append("<p>Сгенерировано автоматически. Импорт модулей не выполняется.</p>")
    for mod in modules:
        parts.append("<div class='mod'>")
        parts.append(f"<div><strong><a href='{html.escape(mod.name)}.html'>{html.escape(mod.name)}</a></strong></div>")
        if mod.docstring:
            short = mod.docstring.strip().splitlines()[0]
            parts.append(f"<div>{html.escape(short)}</div>")
        parts.append("</div>")
    parts.append("</body></html>")
    (output_dir / "index.html").write_text("\n".join(parts), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-dir", default=".", help="Папка, в которой лежат .py файлы.")
    parser.add_argument("--output-dir", default="../python_docs_html", help="Куда сохранять HTML.")
    args = parser.parse_args()

    source_dir = Path(args.source_dir).resolve()
    output_dir = (source_dir / args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    py_files = sorted(source_dir.glob("*.py"))
    py_files = [p for p in py_files if p.name not in {"generate_html_docs.py"}]

    modules: List[ModuleDoc] = []
    for p in py_files:
        modules.append(parse_module(p))

    for m in modules:
        render_module_html(m, output_dir)

    render_index_html(modules, output_dir)
    print(f"HTML docs generated: {output_dir}")


if __name__ == "__main__":
    main()

