#include "remoteConsole.h"

RemoteConsole::RemoteConsole(QWidget* parent) :QWidget(parent)
{
	hlayout = new QHBoxLayout;
	vlayout = new QVBoxLayout;
	QIcon arrow_icon(QApplication::applicationDirPath() + "/" + "arrow_image.png");
	button_send = new QPushButton(arrow_icon,"");
	connect(button_send, &QPushButton::clicked, this, &RemoteConsole::slotSendCommand);
	console = new QTextEdit;
	command_line = new QLineEdit;
	help = new QCheckBox("help",this);
	connect(help, &QCheckBox::clicked, this, &RemoteConsole::showHelp);
	menu_variant = new QMenu(this);
	menu_variant->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

	hlayout->addWidget(help);
	hlayout->addWidget(command_line);
	hlayout->addWidget(button_send);
	vlayout->addWidget(console);
	vlayout->addLayout(hlayout);
	setLayout(vlayout);

	prefix_tree = new PrefixTree();
	fillPrefixTree();
}

void RemoteConsole::fillPrefixTree()
{
	QString console_command_str = "cmd\ncopy\ncd\ncall\ncmdkey\ncls\nchange\ndir\nmkdir\nrd\nrmdir\nfind\nformat\nmove\npath\nmd\n";
	QStringList list_command = console_command_str.toLower().split("\n");
	for (auto el : list_command) {
		prefix_tree->add(el.toStdString());
	}
}

void RemoteConsole::consoleAddText(QString text)
{
	console->append(text);
}

bool RemoteConsole::event(QEvent* event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* key_event = dynamic_cast<QKeyEvent*>(event);
		if (key_event->key() + 1 == Qt::Key_Enter) {
			button_send->clicked();
		}
		else if(key_event->key() == Qt::Key_Up) {
			if (commands_history.size() <= 0) {
				return false;
			}
			if (it_command != commands_history.begin())
				std::advance(it_command, -1);
			command_line->setText(*it_command);
		}
	}
	else if (event->type() == QEvent::Paint) {
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}
	return true;
}

void RemoteConsole::slotTextPaste()
{
	QAction* action = static_cast<QAction*>(sender());
	command_line->setText(action->text());
}

void RemoteConsole::showHelp(bool check)
{
	if (check) {
		connect(command_line, &QLineEdit::textChanged, this, &RemoteConsole::slotOpenPrefixMenu);
	}
	else {
		disconnect(command_line, &QLineEdit::textChanged, this, &RemoteConsole::slotOpenPrefixMenu);
	}
}

void RemoteConsole::slotOpenPrefixMenu()
{
	menu_variant->clear();
	std::vector<std::string> variant_vector = prefix_tree->getWord(command_line->text().toStdString());
	for (auto el : variant_vector) {
		const char* ch = el.c_str();
		menu_variant->addAction(ch,this,&RemoteConsole::slotTextPaste);
	}
	if (variant_vector.empty()) {
		command_line->setFocus();
		return;
	}
	menu_variant->popup(command_line->mapToGlobal(QPoint(0, 25)));
	menu_variant->show();
}

void RemoteConsole::slotSendCommand() {
	commands_history.push_back(replacementText(command_line->text()));
	emit signal_sendCommand(replacementText(command_line->text()));
	it_command = commands_history.end();
	command_line->clear();
}