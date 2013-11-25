#include "discountmarkdownconverter.h"

extern "C" {
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#include <discount/mkdio.h>
}

#include "markdowndocument.h"

class DiscountMarkdownDocument : public MarkdownDocument
{
public:
    explicit DiscountMarkdownDocument(MMIOT *document) : discountDocument(document) {}
    ~DiscountMarkdownDocument() { mkd_cleanup(discountDocument); }

    MMIOT *document() const { return discountDocument; }

private:
    MMIOT *discountDocument;
};


DiscountMarkdownConverter::DiscountMarkdownConverter()
{
}

MarkdownDocument *DiscountMarkdownConverter::createDocument(const QString &text)
{
    MMIOT *doc = 0;

    if (text.length() > 0) {
        QString markdownText(text);

        // text has to always end with a line break,
        // otherwise characters are missing in HTML
        if (!markdownText.endsWith('\n')) {
            markdownText.append('\n');
        }

        QByteArray utf8Data = markdownText.toUtf8();
        doc = mkd_string(utf8Data, utf8Data.length(), 0/*options*/);

        mkd_compile(doc, 0/*options*/);
    }

    return new DiscountMarkdownDocument(doc);
}

QString DiscountMarkdownConverter::renderAsHtml(MarkdownDocument *document)
{
    QString html;

    if (document) {
        DiscountMarkdownDocument *doc = dynamic_cast<DiscountMarkdownDocument*>(document);

        if (doc->document()) {
            char *out;
            mkd_document(doc->document(), &out);

            html = QString::fromUtf8(out);
        }
    }

    return html;
}

QString DiscountMarkdownConverter::renderAsTableOfContents(MarkdownDocument *document)
{
    QString toc;

    if (document) {
        DiscountMarkdownDocument *doc = dynamic_cast<DiscountMarkdownDocument*>(document);

        if (doc->document()) {
            // generate table of contents
            char *out;
            mkd_toc(doc->document(), &out);

            toc = QString::fromUtf8(out);
        }
    }

    return toc;
}
