#ifndef SHADERCODEDIALOG_H
#define SHADERCODEDIALOG_H

#include <QDialog>

class QPushButton;
class QTextEdit;

class ShaderCodeDialog : public QDialog
{
public:
    ShaderCodeDialog(QWidget *parent = 0);
    ~ShaderCodeDialog();

    void setContent(QString code);

private:
    QTextEdit *m_content;
    QPushButton *m_okButton;
};

#endif // SHADERCODEDIALOG_H
