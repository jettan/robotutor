#include "StdAfx.h"

ScriptHighlighter::ScriptHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
	behaviour_format_.setForeground(Qt::blue);
	behaviour_format_.setFontWeight(QFont::Bold);
	HighlightingRule command_rule;
	command_rule.pattern = QRegExp("\\{[^}]+\\}");
	command_rule.format = behaviour_format_;
	highlighting_rules_.append(command_rule);

	behaviour_format_.setForeground(Qt::darkGreen);
	//behaviour_format_.setFontWeight(QFont::Bold);
	HighlightingRule comment_rule;
	comment_rule.pattern = QRegExp("#[^\n]*(\n|$)");
	comment_rule.format = behaviour_format_;
	highlighting_rules_.append(comment_rule);
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