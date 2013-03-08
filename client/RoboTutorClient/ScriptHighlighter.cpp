#include "ScriptHighlighter.h"

ScriptHighlighter::ScriptHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
	behaviour_format_.setForeground(Qt::darkGreen);
	behaviour_format_.setFontWeight(QFont::Bold);
	HighlightingRule rule;
	rule.pattern = QRegExp("\\{[^}].*[^}]*\\}");
	rule.format = behaviour_format_;
	highlighting_rules_.append(rule);
}

ScriptHighlighter::~ScriptHighlighter(void) {}

void ScriptHighlighter::highlightBlock(const QString &text) {
	foreach (const HighlightingRule &rule, highlighting_rules_) {
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0) {
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
}