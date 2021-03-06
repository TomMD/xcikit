# -*- coding: utf-8 -*-
"""
    Pygments lexer for Fire Script
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Based on: pygments.lexers.c_like

    https://pygments.org/docs/lexerdevelopment/

    :copyright: Copyright 2006-2019 by the Pygments team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

from pygments.lexer import RegexLexer, include, bygroups, inherit, words, \
    default
from pygments.token import Text, Comment, Operator, Keyword, Name, String, \
    Number, Punctuation, Error

from pygments.lexers.c_cpp import CLexer

__all__ = ['FireLexer']


class FireLexer(CLexer):
    """
    For `Fire Script <https://github.com/rbrich/xcikit>`_.

    .. versionadded:: 2.0
    """
    name = 'Fire'
    aliases = ['fire']
    filenames = ['*.fire']
    mimetypes = ['text/x-fire-script']

    tokens = {
        'statements': [
            (r'(L?)(")', bygroups(String.Affix, String), 'string'),
            (r"(L?)(')(\\.|\\[0-7]{1,3}|\\x[a-fA-F0-9]{1,2}|[^\\\'\n])(')",
             bygroups(String.Affix, String.Char, String.Char, String.Char)),
            (r'(\d+\.\d*|\.\d+|\d+)[eE][+-]?\d+[LlUu]*', Number.Float),
            (r'(\d+\.\d*|\.\d+|\d+[fF])[fF]?', Number.Float),
            (r'0x[0-9a-fA-F]+[LlUu]*', Number.Hex),
            (r'0[0-7]+[LlUu]*', Number.Oct),
            (r'\d+[LlUu]*', Number.Integer),
            (r'\*/', Error),
            (r'[~!%^&*+=|?:<>/-]', Operator),
            (r'[()\[\],.]', Punctuation),
            (words((
                'else', 'fun', 'import', 'in', 'if', 'then', 'module',
                ), suffix=r'\b'),
             Keyword),
            (words(('Void', 'Bool', 'Byte', 'Char', 'Int', 'Int32', 'Int64',
                    'Float', 'Float32', 'Float64', 'String'), suffix=r'\b'),
             Keyword.Type),
            (r'(true|false|void)\b', Name.Builtin),
            (r'([a-zA-Z_]\w*)(\s*)(:)(?!:)', bygroups(Name.Label, Text, Punctuation)),
            (r'[a-z_]\w*', Name),
            (r'[A-Z]\w*', Name.Class),
        ],
    }
