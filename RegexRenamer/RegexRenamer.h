#pragma once

#include <qboxlayout.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qgridlayout.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qlistwidget.h>
#include <qfileinfo.h>
#include <qregularexpression.h>



class RegexRenamer : public QWidget
{
    Q_OBJECT

public:
    RegexRenamer(QWidget *parent = nullptr);
    ~RegexRenamer();

public slots:
	void onBrowseSrc();
	void onRename();
	void onRenameUpdate();

private:
	bool renamable{ false };
	// member value
	QMap<QString, QString> rename_map;

	// utility function
	void updatePreview(QFileInfoList);
	QSet<QString> check_unique_name(QStringList);

	// widget ------------------------------
	QHBoxLayout layout_main{ this };
	QVBoxLayout layout_property{ this };

	// FileBrowserUI
	QLabel label_src{ "SrcDir :", this };
	QLineEdit edit_src{ this };
	QPushButton btn_src{ "Browse", this };
	QGridLayout layout_browse{ this };

	// RenamerContext
	QLabel label_regex{"Regex :", this };
	QLabel label_repl{ "Repl  :", this };
	QLineEdit edit_regex{ this };
	QLineEdit edit_repl{ this };
	QHBoxLayout layout_rename{ this };

	// prefix
	QHBoxLayout layout_prefix{ this };
	QLabel label_prefix_str{ "PrefixStr:", this };
	QLineEdit edit_prefix_str{ this };
	QLabel label_prefix_nums{ "nums:", this };
	QSpinBox spin_prefix_nums{ this };


	// suffix
	QHBoxLayout layout_suffix{ this };
	QLabel label_suffix_str{ "SuffixStr:", this };
	QLineEdit edit_suffix_str{ this };
	QLabel label_suffix_nums{ "nums:", this };
	QSpinBox spin_suffix_nums{ this };


	// Rename mod
	QPushButton btn_rename{ "Rename", this };
	QListWidget lst_rename_preview{ this };
	

};
