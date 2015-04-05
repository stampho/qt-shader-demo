#include "shadercodedialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>

ShaderCodeDialog::ShaderCodeDialog(QWidget *parent)
    : QDialog(parent)
    , m_content(new QTextEdit(this))
    , m_okButton(new QPushButton("Ok", this))
{
    setMinimumWidth(640);
    m_content->setReadOnly(true);

    connect(m_okButton, SIGNAL(pressed()), this, SLOT(accept()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_content);
    layout->addWidget(m_okButton);
    setLayout(layout);
}

ShaderCodeDialog::~ShaderCodeDialog()
{

}

void ShaderCodeDialog::setContent(QString code)
{
    m_content->setText(code);
}

