#include "RegexRenamer.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qcoreapplication.h>


RegexRenamer::RegexRenamer(QWidget *parent)
    : QWidget(parent)
{
	//icon setting
		setWindowTitle("RegexRenamer");
		setWindowIcon(QIcon(":/Icon/Icon/RegexRenamerIco.ico"));

		layout_browse.addWidget(&label_src, 0, 0);
		layout_browse.addWidget(&edit_src, 0, 1);
		layout_browse.addWidget(&btn_src, 0, 2);

		// regex layout
		layout_rename.addWidget(&label_regex);
		layout_rename.addWidget(&edit_regex);
		layout_rename.addWidget(&label_repl);
		layout_rename.addWidget(&edit_repl);

		// prefix 
		layout_prefix.addWidget(&label_prefix_str);
		layout_prefix.addWidget(&edit_prefix_str);
		layout_prefix.addWidget(&label_prefix_nums);
		layout_prefix.addWidget(&spin_prefix_nums);

		layout_suffix.addWidget(&label_suffix_str);
		layout_suffix.addWidget(&edit_suffix_str);
		layout_suffix.addWidget(&label_suffix_nums);
		layout_suffix.addWidget(&spin_suffix_nums);

		layout_property.addLayout(&layout_browse);
		layout_property.addLayout(&layout_rename);
		layout_property.addLayout(&layout_prefix);
		layout_property.addLayout(&layout_suffix);
		layout_property.addWidget(&btn_rename);

		layout_main.addLayout(&layout_property);
		layout_main.addWidget(&lst_rename_preview);

		setLayout(&layout_main);


		// connect
		connect(&btn_src, &QPushButton::clicked, 
			this, &RegexRenamer::onBrowseSrc);

		connect(&edit_src, &QLineEdit::textChanged,
			this, &RegexRenamer::onRenameUpdate);	

		connect(&edit_regex, &QLineEdit::textChanged,
			this, &RegexRenamer::onRenameUpdate);	

		connect(&edit_repl, &QLineEdit::textChanged,
			this, &RegexRenamer::onRenameUpdate);	

		connect(&spin_prefix_nums, QOverload<int>::of(&QSpinBox::valueChanged),
			this, &RegexRenamer::onRenameUpdate);

		connect(&spin_suffix_nums, QOverload<int>::of(&QSpinBox::valueChanged),
			this, &RegexRenamer::onRenameUpdate);

		connect(&edit_prefix_str, &QLineEdit::textChanged,
			this, &RegexRenamer::onRenameUpdate);

		connect(&edit_suffix_str, &QLineEdit::textChanged,
			this, &RegexRenamer::onRenameUpdate);

		connect(&btn_rename, &QPushButton::clicked,
			this, &RegexRenamer::onRename);

		edit_src.setText(QDir::currentPath());

		onRenameUpdate();
}

RegexRenamer::~RegexRenamer()
{}

void RegexRenamer::onBrowseSrc() {
	QString dir = QFileDialog::getExistingDirectory(
		this, tr("Open Directory"), 
		QDir::currentPath());

	if (!dir.isEmpty()) {
		edit_src.setText(dir);
	}
}


void RegexRenamer::onRenameUpdate()
{
	QString srcDir = edit_src.text();

	// system directory or current application directory is not allowed
	if (srcDir.toLower().trimmed()
			== QString("C:/WINDOWS/system32").toLower().trimmed()) {
		return;
	}

	if (srcDir.toLower().trimmed()
		== QCoreApplication::applicationDirPath().toLower().trimmed()) {
		return;
	}



	QDir dir(srcDir);
	if (!dir.exists()) {
		QMessageBox::warning(this, "Warning", "Directory does not exist");
		return;
	}

   auto src_file_list = dir.entryInfoList(QDir::Files);
	updatePreview(src_file_list);
}

void RegexRenamer::updatePreview(QFileInfoList src_file_list)
{
	
	renamable = true;

	QRegularExpression regex(edit_regex.text());
	QString repl = edit_repl.text();

	int index = 0;
	QString prefix_num = "";
	QString suffix_num = "";
	QString prefix_str = "";
	QString suffix_str = "";

	// update rename map
	rename_map.clear();
	for (auto& file : src_file_list) {
		// split extention
		auto file_name = file.fileName();
		auto file_name_body =  file_name.split(".")[0];
		auto file_name_ext = file_name.split(".")[1];

		// prefix_sufix_num
		if (spin_prefix_nums.value() > 0) {
			prefix_num = QString("%1_")
				.arg(index,spin_prefix_nums.value(),10, QChar('0'));
		}

		if (spin_suffix_nums.value() > 0) {
			suffix_num = QString("_%1")
				.arg(index,spin_suffix_nums.value(),10, QChar('0'));
		}

		// prefix_sufix_str
		if (!edit_prefix_str.text().isEmpty())
			prefix_str = edit_prefix_str.text() + "_";

		if (!edit_suffix_str.text().isEmpty())
			suffix_str = "_" + edit_suffix_str.text();
		

		// regex replace
		auto result = file_name_body.replace(regex, repl);

		rename_map[file_name] = 
			prefix_num + prefix_str + result + suffix_str + suffix_num  
			+ "." +file_name_ext;

		index++;
	}

	// check unique name
	auto duplicate_set = check_unique_name(rename_map.values());

	// preview rendering
	lst_rename_preview.clear();
	for (auto it = rename_map.begin(); it != rename_map.end(); ++it) {
		auto& name = it.key();
		auto& rename = it.value();

		if (duplicate_set.contains(rename)) {
			renamable = false;

			QListWidgetItem* item = new QListWidgetItem(name + " -> " + rename);
			item->setForeground(Qt::red);

			lst_rename_preview.addItem(item);
		}else
		{
			lst_rename_preview.addItem(name + " -> " + rename);
		}
	}
}

// get duplicate name
QSet<QString> RegexRenamer::check_unique_name(QStringList rename_list)
{
	QSet<QString> unique_set;
	QSet<QString> duplicate_set;

	for (auto& name : rename_list) {
		if (unique_set.contains(name)) {
			duplicate_set.insert(name);
		}
		else {
			unique_set.insert(name);
		}
	}
	return duplicate_set;
}


void RegexRenamer::onRename()
{
	// check renamable
	if (!renamable) {
		QMessageBox::warning(this, "Warning", "Rename is not possible");
		return;
	}
	
	// check src dir
	QString srcDir = edit_src.text();
	QDir dir(srcDir);
	if (!dir.exists()) {
		QMessageBox::warning(this, "Warning", "Directory does not exist");
		return;
	}

	// rename
	for (auto it = rename_map.begin(); it != rename_map.end(); ++it) {
		auto& name = it.key();
		auto& rename = it.value();

		auto src = srcDir + "/" + name;
		auto dst = srcDir + "/" + rename;

		if (!QFile::rename(src, dst)) {
			QMessageBox::warning(this, "Warning", "Rename failed");
		}
	}

	// update preview
	onRenameUpdate();
}
