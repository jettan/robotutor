#pragma once

struct HighlightingRule {
	QRegExp pattern;
	QTextCharFormat format;
};

class ScriptHighlighter : public QSyntaxHighlighter {
public:
	ScriptHighlighter(QTextDocument *parent);
	~ScriptHighlighter(void);

	void highlightBlock(const QString &text);

private:
	QVector<HighlightingRule> highlighting_rules_;

	QTextCharFormat behaviour_format_;
};
