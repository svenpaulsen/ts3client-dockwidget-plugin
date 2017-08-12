/**
 * TeamSpeak 3 Client Notification Plugin
 *
 * Copyright (c) Sven Paulsen. All rights reserved.
 */

#include "widgethelper.h"

/**
 * The WidgetHelper object constructor.
 */
WidgetHelper::WidgetHelper() : Singleton<WidgetHelper>()
{
    QWidgetList widgets = qApp->allWidgets();

    for(QWidgetList::iterator i = widgets.begin(); i != widgets.end(); ++i)
    {
        if((*i)->inherits("QMainWindow")) m_mainWindow = (QMainWindow*) (*i);
    }
}

/**
 * The WidgetHelper object destructor.
 */
WidgetHelper::~WidgetHelper()
{}

/**
 * Hooks into the client UI and tries to find key widgets to interact with.
 */
unsigned int WidgetHelper::hook()
{
    if(!m_mainWindow) return 1;

    if(!m_serverTabs)
    {
        if((m_serverTabs = getWidget<QTabWidget*>("ServerViewManager", m_mainWindow)))
        {
            connect(m_serverTabs, &QTabWidget::currentChanged, this, &WidgetHelper::onServerTabChanged);
            connect(m_serverTabs, &QTabWidget::tabBarClicked, this, &WidgetHelper::onServerTabClicked);
            connect(m_serverTabs, &QTabWidget::tabBarDoubleClicked, this, &WidgetHelper::onServerTabDoubleClicked);
        }
    }

    if(!m_serverArea)
    {
        m_serverArea = getWidget<QWidget*>("MainWindowServerTabsWidget", m_mainWindow);
    }

    if(!m_serverStack)
    {
        if(m_serverArea) m_serverStack = m_serverArea->findChild<QStackedWidget*>();
    }

    if(!m_chatTabs)
    {
        if((m_chatTabs = getWidget<QTabWidget*>("ChatTabWidget", m_mainWindow)))
        {
            connect(m_chatTabs, &QTabWidget::currentChanged, this, &WidgetHelper::onChatTabChanged);
            connect(m_chatTabs, &QTabWidget::tabBarClicked, this, &WidgetHelper::onChatTabClicked);
            connect(m_chatTabs, &QTabWidget::tabBarDoubleClicked, this, &WidgetHelper::onChatTabDoubleClicked);
        }
    }

    if(!m_chatArea)
    {
        m_chatArea = getWidget<QWidget*>("MainWindowChatWidget", m_mainWindow);
    }

    if(!m_chatStack)
    {
        if(m_chatArea) m_chatStack = m_chatArea->findChild<QStackedWidget*>();
    }

    if(!m_splitter)
    {
        m_splitter = m_mainWindow->findChild<QSplitter*>();
    }

    if(!m_hooked && m_hooks++ < 25)
    {
        QTimer::singleShot(200, this, SLOT(onPluginHookFailed()));
    }

    if(m_serverTabs && m_serverArea && m_chatTabs && m_chatArea && m_splitter && m_serverStack && m_chatStack)
    {
        m_hooked = true;

        start();
        restore();
    }

    return 0;
}

/**
 * Unhooks from the client UI.
 */
unsigned int WidgetHelper::unhook()
{
    if(!m_hooked) return 1;

    backup();
    stop();

    m_hooked = false;

    delete m_stacked;
    delete m_dockInfo;
    delete m_dockChat;

    return 0;
}

/**
 * Writes plugin settings back to the config file.
 */
unsigned int WidgetHelper::backup()
{
    if(!m_loaded) return 0;

    char path[BUFFSIZE_S];

    pluginSDK.getConfigPath(path, BUFFSIZE_S);

    QSettings conf(QString(path) + "plugin_dockwidget.ini", QSettings::IniFormat);

    pluginSDK.logMessage(QString("Writing plugin settings: %0").arg(conf.fileName()).toUtf8().data(), LogLevel_INFO, PLUGIN_NAME, 0);

    conf.setValue("mainGeometry", m_mainWindow->saveGeometry());
    conf.setValue("mainWinState", m_mainWindow->saveState());
    conf.setValue("chatFloating", m_dockChat->isFloating());
    conf.setValue("chatGeometry", m_dockChat->saveGeometry());
    conf.setValue("infoFloating", m_dockInfo->isFloating());
    conf.setValue("infoGeometry", m_dockInfo->saveGeometry());

    m_loaded = false;

    return conf.status();
}

/**
 * Reads plugin settings from the config file.
 */
unsigned int WidgetHelper::restore()
{
    if(m_loaded) return 0;

    char path[BUFFSIZE_S];

    pluginSDK.getConfigPath(path, BUFFSIZE_S);

    QSettings conf(QString(path) + "plugin_dockwidget.ini", QSettings::IniFormat);

    if(conf.childKeys().size())
    {
        pluginSDK.logMessage(QString("Reading plugin settings: %0").arg(conf.fileName()).toUtf8().data(), LogLevel_INFO, PLUGIN_NAME, 0);

        m_mainWindow->restoreGeometry(conf.value("mainGeometry", m_mainWindow->saveGeometry()).toByteArray());
        m_mainWindow->restoreState(conf.value("mainWinState", m_mainWindow->saveState()).toByteArray());
        m_dockChat->setFloating(conf.value("chatFloating", m_dockChat->isFloating()).toBool());
        m_dockChat->restoreGeometry(conf.value("chatGeometry", m_dockChat->saveGeometry()).toByteArray());
        m_dockInfo->setFloating(conf.value("infoFloating", m_dockInfo->isFloating()).toBool());
        m_dockInfo->restoreGeometry(conf.value("infoGeometry", m_dockInfo->saveGeometry()).toByteArray());

        m_dockChat->show();
        m_dockInfo->show();
    }

    m_loaded = true;

    return conf.status();
}

/**
 * Starts manipulating the client UI by adding dock widget areas and making the chat and info frame areas dockable.
 */
unsigned int WidgetHelper::start()
{
    if(m_docked) return 1;

    m_dockInfo = new QDockWidget(qApp->translate("MainWindow", "Information"));
    m_dockChat = new QDockWidget(qApp->translate("ChatSetupDialog", "Chat"));
    m_stacked  = new QStackedWidget(m_dockInfo);

    m_dockInfo->setWidget(m_stacked);
    m_dockInfo->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_dockInfo->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_dockInfo->setObjectName("infoFrameDock");

    m_dockChat->setWidget(m_chatArea);
    m_dockChat->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_dockChat->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_dockChat->setObjectName("chatAreaDock");

    m_mainWindow->setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
    m_mainWindow->addDockWidget(Qt::RightDockWidgetArea, m_dockInfo);
    m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, m_dockChat);

    if(QWidget* serverView = m_serverTabs->currentWidget())
    {
        if(QWidget* infoFrame = serverView->findChild<QWidget*>())
        {
            m_widgets.insert(serverView, m_stacked->addWidget(infoFrame));
            m_stacked->setCurrentWidget(infoFrame);

            connect(serverView, &QObject::destroyed, this, &WidgetHelper::onServerTabDestroyed);
        }
    }

    m_docked = true;

    return 0;
}

/**
 * Stops manipulating the client UI and restores the original state of all widgets.
 */
unsigned int WidgetHelper::stop()
{
    if(!m_docked) return 1;

    m_mainWindow->removeDockWidget(m_dockInfo);
    m_mainWindow->removeDockWidget(m_dockChat);

    m_serverTabs->setTabPosition(QTabWidget::North);

    m_splitter->addWidget(m_chatArea);

    for(QMap<QWidget*, int>::const_iterator i = m_widgets.begin(); i != m_widgets.end(); ++i)
    {
        QWidget* infoFrame = m_stacked->widget(i.value());

        m_stacked->removeWidget(infoFrame);

        if(m_mainWindow->isVisible())
        {
            infoFrame->setParent(i.key());
            infoFrame->show();
        }
    }

    m_docked = false;

    return 0;
}

/**
 * Called when the plugin fails to hook for a maximum of 25 retries.
 */
void WidgetHelper::onPluginHookFailed()
{
    hook();
}

/**
 * Called when the active server tab has been changed.
 */
void WidgetHelper::onServerTabChanged(int index)
{
    if(!m_docked || !m_hooked) return;

    if(QWidget* serverView = m_serverTabs->widget(index))
    {
        if(m_widgets.contains(serverView))
        {
            m_stacked->setCurrentWidget(m_stacked->widget(m_widgets[serverView]));
        }
        else
        {
            if(QWidget* infoFrame = serverView->findChild<QWidget*>())
            {
                m_widgets.insert(serverView, m_stacked->addWidget(infoFrame));
                m_stacked->setCurrentWidget(infoFrame);

                connect(serverView, &QObject::destroyed, this, &WidgetHelper::onServerTabDestroyed);
            }
        }
    }
}

/**
 * Called when a server tab is clicked.
 */
void WidgetHelper::onServerTabClicked(int index)
{
    if(!m_docked || !m_hooked) return;

    // not implemented

    Q_UNUSED(index)
}

/**
 * Called when a server tab is double-clicked.
 */
void WidgetHelper::onServerTabDoubleClicked(int index)
{
    if(!m_docked || !m_hooked) return;

    // not implemented

    Q_UNUSED(index)
}

/**
 * Called when a server tab has been closed.
 */
void WidgetHelper::onServerTabDestroyed(QObject* obj)
{
    if(!m_docked || !m_hooked) return;

    if(m_widgets.contains((QWidget*) obj))
    {
        QWidget* infoFrame = m_stacked->widget(m_widgets[(QWidget*) obj]);

        m_widgets.remove((QWidget*) obj);
        m_stacked->removeWidget(infoFrame);

        delete infoFrame;
    }
}

/**
 * Called when the active chat tab has been changed.
 */
void WidgetHelper::onChatTabChanged(int index)
{
    if(!m_docked || !m_hooked) return;

    // not implemented

    Q_UNUSED(index)
}

/**
 * Called when a chat tab is clicked.
 */
void WidgetHelper::onChatTabClicked(int index)
{
    if(!m_docked || !m_hooked) return;

    // not implemented

    Q_UNUSED(index)
}

/**
 * Called when a chat tab is double-clicked.
 */
void WidgetHelper::onChatTabDoubleClicked(int index)
{
    if(!m_docked || !m_hooked) return;

    // not implemented

    Q_UNUSED(index)
}
