/***************************************************************************
**                                                                        **
**  ScriptCommunicator, is a tool for sending/receiving data with several **
**  interfaces.                                                           **
**  Copyright (C) 2014 Stefan Zieker                                      **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Stefan Zieker                                        **
**  Website/Contact: http://sourceforge.net/projects/scriptcommunicator/  **
****************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QFileDialog>
#include "aardvarkI2cSpi.h"
#include <QColorDialog>
#include <QNetworkInterface>
#include <QScrollBar>
#include <QProcess>
#include <QShortcut>
#include "colorWidgets/color_dialog.hpp"

QT_USE_NAMESPACE

/**
 * Constructor.
 * @param actionLockScrolling
 *      Pointer to the lock scrolling action in the main window.
 */
SettingsDialog::SettingsDialog(QAction *actionLockScrolling) :
    m_userInterface(new Ui::SettingsDialog), m_actionLockScrolling(actionLockScrolling), m_interfaceSettingsCanBeChanged(true)
{
    m_userInterface->setupUi(this);

    m_aardvarkI2cGpioGuiElements[0].mode = m_userInterface->aardvarkGpioMode0;
    m_aardvarkI2cGpioGuiElements[0].outValue = m_userInterface->aardvarkGpioOutValue0;
    m_aardvarkI2cGpioGuiElements[0].inValue = m_userInterface->aardvarkGpioInValue0;
    m_aardvarkI2cGpioGuiElements[1].mode = m_userInterface->aardvarkGpioMode1;
    m_aardvarkI2cGpioGuiElements[1].outValue = m_userInterface->aardvarkGpioOutValue1;
    m_aardvarkI2cGpioGuiElements[1].inValue = m_userInterface->aardvarkGpioInValue1;
    m_aardvarkI2cGpioGuiElements[2].mode = m_userInterface->aardvarkGpioMode2;
    m_aardvarkI2cGpioGuiElements[2].outValue = m_userInterface->aardvarkGpioOutValue2;
    m_aardvarkI2cGpioGuiElements[2].inValue = m_userInterface->aardvarkGpioInValue2;
    m_aardvarkI2cGpioGuiElements[3].mode = m_userInterface->aardvarkGpioMode3;
    m_aardvarkI2cGpioGuiElements[3].outValue = m_userInterface->aardvarkGpioOutValue3;
    m_aardvarkI2cGpioGuiElements[3].inValue = m_userInterface->aardvarkGpioInValue3;
    m_aardvarkI2cGpioGuiElements[4].mode = m_userInterface->aardvarkGpioMode4;
    m_aardvarkI2cGpioGuiElements[4].outValue = m_userInterface->aardvarkGpioOutValue4;
    m_aardvarkI2cGpioGuiElements[4].inValue = m_userInterface->aardvarkGpioInValue4;
    m_aardvarkI2cGpioGuiElements[5].mode = m_userInterface->aardvarkGpioMode5;
    m_aardvarkI2cGpioGuiElements[5].outValue = m_userInterface->aardvarkGpioOutValue5;
    m_aardvarkI2cGpioGuiElements[5].inValue = m_userInterface->aardvarkGpioInValue5;


    int width  = -1;
    if(m_userInterface->connectionTypeComboBox->width() > m_userInterface->endianessComboBox->width())
    {
        width = m_userInterface->connectionTypeComboBox->width();
    }
    else
    {
        width = m_userInterface->endianessComboBox->width();
    }
#ifdef Q_OS_MAC
    width = (width < 120) ? 120 : width;
#endif

    m_userInterface->connectionTypeComboBox->setMinimumWidth(width);
    m_userInterface->connectionTypeComboBox->setMaximumWidth(width);
    m_userInterface->endianessComboBox->setMinimumWidth(width);
    m_userInterface->endianessComboBox->setMaximumWidth(width);

    m_pcanInterface = new PCANBasicClass(this);

    QStringList listFontSize;
    for (int fs = Settings::MIN_FONT_SIZE; fs <= Settings::MAX_FONT_SIZE; fs++)
        listFontSize.append(QString::number(fs));

    m_userInterface->consoleFontSizeComboBox->addItems(listFontSize);
    m_userInterface->consoleFontSizeComboBox->setCurrentText("10");

    m_userInterface->htmlLogFontSizeComboBox->addItems(listFontSize);
    m_userInterface->htmlLogFontSizeComboBox->setCurrentText("10");

    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+Shift+X"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

    connect(m_userInterface->closeButton, SIGNAL(clicked()),
            this, SLOT(closeButtonPressed()));

    connect(m_userInterface->ClearTextLogPushButton, SIGNAL(clicked()),
            this, SLOT(deleteTextLogButtonPressedSlot()));

    connect(m_userInterface->logTextLogPushButton, SIGNAL(clicked()),
            this, SLOT(searchTextLogButtonPressedSlot()));

    connect(m_userInterface->ClearHtmlLogPushButton, SIGNAL(clicked()),
            this, SLOT(deleteHtmlLogButtonPressedSlot()));

    connect(m_userInterface->logHtmlLogPushButton, SIGNAL(clicked()),
            this, SLOT(searchHtmlLogButtonPressedSlot()));

    connect(m_userInterface->HtmlLogCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(htmLogActivatedSlot(bool)));

    connect(m_userInterface->TextLogCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(textLogActivatedSlot(bool)));

    connect(m_userInterface->serialPortInfoListBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->serialPortInfoListBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(comPortChangedSlot(QString)));

    connect(m_userInterface->serialPortInfoListBox, SIGNAL(activated(QString)),
            this, SLOT(comPortChangedSlot(QString)));

    m_userInterface->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(m_userInterface->baudRateBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomBaudRatePolicySlot(int)));

    connect(m_userInterface->baudRateBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->dataBitsBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->parityBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->stopBitsBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->flowControlBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->ConsoleBufferLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->PrintTimeStampCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->ShowSendInConsoleCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->ShowReceivedCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->consoleShowAsciiCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->consoleShowBinaryCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->consoleShowCanMetaCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->LogWriteCanMetaCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(actionLockScrolling, SIGNAL(triggered()), this, SLOT(lockScrollingSlot()));

    connect(m_userInterface->LogWriteSendCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->LogWriteReceivedCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->logWithTimeStampCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->logAppendTimestampCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(appendTimestampAtLogsChangedSlot(int)));

    connect(m_userInterface->consoleFontComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->consoleFontSizeComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->htmlLogFontComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->htmlLogFontSizeComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->searchScriptEditorPushButton, SIGNAL(clicked()),
            this, SLOT(searchScriptEditorButtonPressedSlot()));

    connect(m_userInterface->scriptEditorPathLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(scriptEditorPathLineEditTextChangedSlot(QString)));

    connect(m_userInterface->useExternalScriptEditorCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->consoleShowDecimalCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->consoleShowHexCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->logShowBinaryCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->logShowDecimalCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->logShowHexCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->logShowAsciiCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->consoleShowMixedCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->showCanTabCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->connectionTypeComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(conectionTypeChangesSlot(QString)));

    connect(m_userInterface->socketAddressLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->socketPartnerPortLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->socketsTypeComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->PrintTimeStampLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->logWithTimeStampLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));


    connect(m_userInterface->ConsoleUpdateIntervallLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->pcanDevicesComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->pcan5VoltComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->pcanBusOffResetComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->pcanBaudratecomboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->consoleDecimalsType, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->consoleI2cMetadata, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->logI2cMetadata, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->logDecimalsType, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->endianessComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->pcanDetectPushButton, SIGNAL(clicked()),
            this, SLOT(detectPcanSlot()));

    connect(m_userInterface->pcanExtendedRadioButton, SIGNAL(clicked()),
            this, SLOT(setFilterRadioButtonPressedSlot()));

    connect(m_userInterface->pcanStandardRadioButton, SIGNAL(clicked()),
            this, SLOT(setFilterRadioButtonPressedSlot()));

    // connect color buttons via signal mapper to function which opens color dialog
    mapColorButtons = new QSignalMapper(this);
    mapColorButtons->setMapping(m_userInterface->consoleSendColorButton, m_userInterface->consoleSendColorButton);
    mapColorButtons->setMapping(m_userInterface->consoleMessageColorButton, m_userInterface->consoleMessageColorButton);
    mapColorButtons->setMapping(m_userInterface->consoleReceiveColorButton, m_userInterface->consoleReceiveColorButton);
    mapColorButtons->setMapping(m_userInterface->consoleBackgroundColorButton, m_userInterface->consoleBackgroundColorButton);
    mapColorButtons->setMapping(m_userInterface->btnColorAscii, m_userInterface->btnColorAscii);
    mapColorButtons->setMapping(m_userInterface->btnColorDec, m_userInterface->btnColorDec);
    mapColorButtons->setMapping(m_userInterface->btnColorHex, m_userInterface->btnColorHex);
    mapColorButtons->setMapping(m_userInterface->btnColorBin, m_userInterface->btnColorBin);
    connect(m_userInterface->consoleSendColorButton, SIGNAL(clicked()), mapColorButtons, SLOT(map()));
    connect(m_userInterface->consoleMessageColorButton, SIGNAL(clicked()), mapColorButtons, SLOT(map()));
    connect(m_userInterface->consoleReceiveColorButton, SIGNAL(clicked()), mapColorButtons, SLOT(map()));
    connect(m_userInterface->consoleBackgroundColorButton, SIGNAL(clicked()), mapColorButtons, SLOT(map()));
    connect(m_userInterface->btnColorAscii, SIGNAL(clicked()), mapColorButtons, SLOT(map()));
    connect(m_userInterface->btnColorDec, SIGNAL(clicked()), mapColorButtons, SLOT(map()));
    connect(m_userInterface->btnColorHex, SIGNAL(clicked()), mapColorButtons, SLOT(map()));
    connect(m_userInterface->btnColorBin, SIGNAL(clicked()), mapColorButtons, SLOT(map()));
    connect(mapColorButtons, static_cast<void(QSignalMapper::*)(QWidget *)>(&QSignalMapper::mapped),
    [=](QWidget *widget){
        QToolButton *btn = qobject_cast<QToolButton *>(widget);
        colorButtonPressed(btn);
    });

    connect(m_userInterface->consoleNewLineAfterNumberBytes, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->consoleNewLineAfterPause, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->logNewLineAfterNumberBytes, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->logNewLineAfterPause, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->consoleSendOnEnter, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->ConsoleTimestampFormat, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->LogTimestampFormat, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->noProxyRadioButton, SIGNAL(clicked()),
            this, SLOT(socketProxyRadioButtonClickedSlot()));
    connect(m_userInterface->useSystemProxyRadioButton, SIGNAL(clicked()),
            this, SLOT(socketProxyRadioButtonClickedSlot()));
    connect(m_userInterface->useSpecificProxyRadioButton, SIGNAL(clicked()),
            this, SLOT(socketProxyRadioButtonClickedSlot()));

    connect(m_userInterface->updateNoProxy, SIGNAL(clicked()),
            this, SLOT(updateProxyRadioButtonClickedSlot()));
    connect(m_userInterface->updateUseSystemProxy, SIGNAL(clicked()),
            this, SLOT(updateProxyRadioButtonClickedSlot()));
    connect(m_userInterface->updateUseSpecificProxy, SIGNAL(clicked()),
            this, SLOT(updateProxyRadioButtonClickedSlot()));


    connect(m_userInterface->proxyAddressLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->proxyPortLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->proxyUserNameLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->proxyPasswordLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->updateProxyAddress, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->updateProxyPort, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->updateProxyUserName, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->updateProxyPassword, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));


    connect(m_userInterface->aardvarkI2cSpiPort, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkI2cSpiMode, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkI2cSpi5V, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkI2cBaudrate, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkI2cPullUp, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkSpiPolarity, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkSpiBitorder, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkSpiSSPolarity, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkSpiPhase, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));
    connect(m_userInterface->aardvarkSpiBaudrate, SIGNAL(textChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->aardvarkI2cSpiScan, SIGNAL(clicked()),
            this, SLOT(aardvarkI2cSpiScanButtonSlot()));
    connect(m_userInterface->aardvarkI2cFreeBus, SIGNAL(clicked()),
            this, SLOT(aardvarkI2cSpiFreeBusButtonSlot()));

    for(int i = 0; i < AARDVARK_I2C_SPI_GPIO_COUNT; i++)
    {
        connect(m_aardvarkI2cGpioGuiElements[i].mode, SIGNAL(currentTextChanged(QString)),
                this, SLOT(textFromGuiElementChangedSlot(QString)));
        connect(m_aardvarkI2cGpioGuiElements[i].outValue, SIGNAL(currentTextChanged(QString)),
                this, SLOT(textFromGuiElementChangedSlot(QString)));
    }

    m_userInterface->serialPortInfoListBox->installEventFilter(this);

    fillSerialPortParameters();

    updateSettings();

    m_userInterface->ConsoleBufferLineEdit->setValidator(new QIntValidator(MAIN_VALUE_MAX_CHARS_IN_EDIT_BOX, MAX_VALUE_MAX_CHARS_IN_EDIT_BOX, m_userInterface->ConsoleBufferLineEdit));
    QString str = QString("%1").arg(DEFAULT_VALUE_MAX_CHARS_IN_EDIT_BOX);
    m_userInterface->ConsoleBufferLineEdit->setText(str);

    m_userInterface->ConsoleUpdateIntervallLineEdit->setValidator(new QIntValidator(MIN_CONSOLE_UPDATE_INTERVAL, MAX_CONSOLE_UPDATE_INTERVAL, m_userInterface->PrintTimeStampLineEdit));
    str = QString("%1").arg(DEFAULT_CONSOLE_UPDATE_INTERVAL);
    m_userInterface->ConsoleUpdateIntervallLineEdit->setText(str);


    m_userInterface->PrintTimeStampLineEdit->setValidator(new QIntValidator(0, INT_MAX, m_userInterface->PrintTimeStampLineEdit));
    str = QString("%1").arg(DEFAULT_VALUE_TIME_STAMP_INTERVAL);
    m_userInterface->PrintTimeStampLineEdit->setText(str);

    m_userInterface->logWithTimeStampLineEdit->setValidator(new QIntValidator(0, INT_MAX, m_userInterface->logWithTimeStampLineEdit));
    str = QString("%1").arg(DEFAULT_VALUE_TIME_STAMP_INTERVAL);
    m_userInterface->logWithTimeStampLineEdit->setText(str);

    m_userInterface->socketPartnerPortLineEdit->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->socketPartnerPortLineEdit));
    m_userInterface->socketOwnPortLineEdit->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->socketOwnPortLineEdit));
    m_userInterface->proxyPortLineEdit->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->proxyPortLineEdit));

    m_userInterface->consoleNewLineAfterNumberBytes->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->consoleNewLineAfterNumberBytes));
    m_userInterface->consoleNewLineAfterPause->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->consoleNewLineAfterPause));

    m_userInterface->logNewLineAfterNumberBytes->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->logNewLineAfterNumberBytes));
    m_userInterface->logNewLineAfterPause->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->logNewLineAfterPause));

    m_userInterface->aardvarkI2cBaudrate->setValidator(new QIntValidator(0, 800, m_userInterface->aardvarkI2cBaudrate));
    m_userInterface->aardvarkI2cSlaveAddress->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->aardvarkI2cSlaveAddress));
    m_userInterface->aardvarkI2cSpiPort->setValidator(new QIntValidator(0, USHRT_MAX, m_userInterface->aardvarkI2cSpiPort));
    m_userInterface->aardvarkSpiBaudrate->setValidator(new QIntValidator(0, 8000, m_userInterface->aardvarkSpiBaudrate));

    //Read all serial port informations.
    for(auto list : getSerialPortsInfo())
    {
        m_userInterface->serialPortInfoListBox->addItem(list.at(0));
    }

    m_userInterface->tabWidget->setCurrentIndex(0);

    m_userInterface->aardvarkI2cSpiDevices->appendPlainText(AardvarkI2cSpi::detectDevices());

    detectPcanSlot();

    QRegExpValidator* v = new QRegExpValidator(m_userInterface->pcanFilterFromLineEdit);
    QRegExp rx ("[a-f0-9]{1,8}");
    v->setRegExp(rx);
    m_userInterface->pcanFilterFromLineEdit->setValidator(v);

    v = new QRegExpValidator(m_userInterface->pcanFilterToLineEdit);
    v->setRegExp(rx);
    m_userInterface->pcanFilterToLineEdit->setValidator(v);

    setButtonColorFromString("000000", m_userInterface->consoleReceiveColorButton);
    setButtonColorFromString("7c0000", m_userInterface->consoleSendColorButton);
    setButtonColorFromString("efefef", m_userInterface->consoleBackgroundColorButton);
    setButtonColorFromString("7c0000", m_userInterface->consoleMessageColorButton);
    setButtonColorFromString("8faf9f", m_userInterface->btnColorAscii);
    setButtonColorFromString("f8f893", m_userInterface->btnColorDec);
    setButtonColorFromString("6c9339", m_userInterface->btnColorHex);
    setButtonColorFromString("bf9b76", m_userInterface->btnColorBin);

    m_userInterface->consoleNewLineAt->addItem("LF", (quint16)'\n');
    m_userInterface->consoleNewLineAt->addItem("CR", (quint16)'\r');
    m_userInterface->consoleNewLineAt->addItem("None", (quint16)0xffff);
    m_userInterface->consoleNewLineAt->addItem("Custom");
    m_userInterface->consoleNewLineAt->setInsertPolicy(QComboBox::NoInsert);

    connect(m_userInterface->consoleNewLineAt, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomConsoleNewLineAtSlot(int)));
    connect(m_userInterface->consoleNewLineAt, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    m_userInterface->logNewLineAt->addItem("LF", (quint16)'\n');
    m_userInterface->logNewLineAt->addItem("CR", (quint16)'\r');
    m_userInterface->logNewLineAt->addItem("None", (quint16)0xffff);
    m_userInterface->logNewLineAt->addItem("Custom");
    m_userInterface->logNewLineAt->setInsertPolicy(QComboBox::NoInsert);

    connect(m_userInterface->logNewLineAt, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomLogNewLineAtSlot(int)));
    connect(m_userInterface->logNewLineAt, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    m_userInterface->consoleSendOnEnter->addItem("LF", "\n");
    m_userInterface->consoleSendOnEnter->addItem("CR", "\r");
#ifdef Q_OS_WIN32
    m_userInterface->consoleSendOnEnter->addItem("CR+ LF", "\r\n");
#else
    m_userInterface->consoleSendOnEnter->addItem("CR+LF", "\r\n");
#endif
    m_userInterface->consoleSendOnEnter->addItem("None", "");

    m_userInterface->consoleTimestampAtByteComboBox->addItem("LF", (quint16)'\n');
    m_userInterface->consoleTimestampAtByteComboBox->addItem("CR", (quint16)'\r');
    m_userInterface->consoleTimestampAtByteComboBox->addItem("Custom");
    m_userInterface->consoleTimestampAtByteComboBox->setInsertPolicy(QComboBox::NoInsert);
    connect(m_userInterface->consoleTimestampAtByteComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomConsoleTimestampAtSlot(int)));
    connect(m_userInterface->consoleTimestampAtByteComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    m_userInterface->logTimestampAtByteComboBox->addItem("LF", (quint16)'\n');
    m_userInterface->logTimestampAtByteComboBox->addItem("CR", (quint16)'\r');
    m_userInterface->logTimestampAtByteComboBox->addItem("Custom");
    m_userInterface->logTimestampAtByteComboBox->setInsertPolicy(QComboBox::NoInsert);
    connect(m_userInterface->logTimestampAtByteComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomLogTimestampAtSlot(int)));
    connect(m_userInterface->logTimestampAtByteComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(textFromGuiElementChangedSlot(QString)));

    connect(m_userInterface->consoleTimestampAtByteCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    connect(m_userInterface->logTimestampAtByteCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(stateFromCheckboxChangedSlot(int)));

    m_userInterface->serialPortInfoListBox->setAutoCompletion(false);
    m_userInterface->consoleNewLineAt->setAutoCompletion(false);;
    m_userInterface->logNewLineAt->setAutoCompletion(false);

    setWindowTitle("ScriptCommunicator " + MainWindow::VERSION + " - Settings");
}

/**
 * Destructor.
 */
SettingsDialog::~SettingsDialog()
{
    delete m_userInterface;
}


/**
 * Shows (socket tab) all local ip addresses found on this PC.
 */
void SettingsDialog::showAllLocalIpAddresses(void)
{
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    m_userInterface->iPAddressesTextEdit->clear();
    m_userInterface->iPAddressesTextEdit->append("local IPv6 and IPv4 addresses:");
    m_userInterface->iPAddressesTextEdit->append("");

    for (int i = 0; i < ipList.size(); ++i)
    {
        m_userInterface->iPAddressesTextEdit->append(ipList.at(i).toString());
    }
    m_userInterface->iPAddressesTextEdit->moveCursor (QTextCursor::Start);
}

/**
 * Is called if a color button button is pressed.
 *
 * @param button
 *      The pressed button.
 */
void SettingsDialog::colorButtonPressed(QToolButton* button)
{
    QPalette palette = button->palette();

    color_widgets::ColorDialog* dialog = new color_widgets::ColorDialog(this);
    dialog->setButtonMode(color_widgets::ColorDialog::OkCancel);
    dialog->setColor(palette.color(QPalette::Button));
    dialog->setAlphaEnabled(false);

    if(dialog->exec())
    {
        QColor color = dialog->color();

        button->setStyleSheet(QString("background-color: %1;}").arg(color.name()));
        updateSettings();
        emit configHasToBeSavedSignal();
    }
}

/**
 * Is called if a filter radio button is pressed.
 */
void SettingsDialog::setFilterRadioButtonPressedSlot(void)
{
    QRegExpValidator* v1 = new QRegExpValidator(m_userInterface->pcanFilterFromLineEdit);
    QRegExpValidator* v2 = new QRegExpValidator(m_userInterface->pcanFilterToLineEdit);

    if(m_userInterface->pcanStandardRadioButton->isChecked())
    {

        QRegExp rx("[a-f0-9]{1,3}");
        v1->setRegExp(rx);
        m_userInterface->pcanFilterFromLineEdit->setText("000");


        v2->setRegExp(rx);
        m_userInterface->pcanFilterToLineEdit->setText("7ff");
    }
    else
    {
        QRegExp rx("[a-f0-9]{1,8}");
        v1->setRegExp(rx);
        m_userInterface->pcanFilterFromLineEdit->setText("00000000");

        v2->setRegExp(rx);
        m_userInterface->pcanFilterToLineEdit->setText("1fffffff");
    }

    m_userInterface->pcanFilterFromLineEdit->setValidator(v1);
    m_userInterface->pcanFilterToLineEdit->setValidator(v2);
}

/**
 * Returns the current pcan baudrate;
 * @return
 *      The current baudrate.
 */
quint16 SettingsDialog::getPcanBaudrate()
{
    TPCANBaudrate result =  PCANBasicClass::convertBaudrateString(m_userInterface->pcanBaudratecomboBox->currentText());
    return result;
}

/**
 * Converts a pcan baudrate.
 * @param baudrate
 *      The baudrate.
 * @return
 *      The converted baudrate.
 */
QString SettingsDialog::convertPcanBaudrate(quint16 baudrate)
{
    QString result;

    switch (baudrate)
    {
    case PCAN_BAUD_1M: {result = "1000";break;}
    case PCAN_BAUD_800K: {result = "800";break;}
    case PCAN_BAUD_500K: {result = "500";break;}
    case PCAN_BAUD_250K: {result = "250";break;}
    case PCAN_BAUD_125K: {result = "125";break;}
    case PCAN_BAUD_100K: {result = "100";break;}
    case PCAN_BAUD_95K: {result = "95";break;}
    case PCAN_BAUD_83K: {result = "83";break;}
    case PCAN_BAUD_50K: {result = "50";break;}
    case PCAN_BAUD_47K: {result = "47";break;}
    case PCAN_BAUD_33K: {result = "33";break;}
    case PCAN_BAUD_20K: {result = "20";break;}
    case PCAN_BAUD_10K: {result = "10";break;}
    case PCAN_BAUD_5K: {result = "5";break;}
    default: {result = "5";break;}
    }

    return result;
}

/**
 * Detects pcan usb interface.
 */
void SettingsDialog::detectPcanSlot(void)
{

    QStringList list;
    int iBuffer;
    int counter = 1;

    for(int i = PCAN_USBBUS1; i <= PCAN_USBBUS8; i++)
    {
        TPCANStatus stsResult = m_pcanInterface->getValue(i, PCAN_CHANNEL_CONDITION, (void*)&iBuffer, sizeof(iBuffer));

        if ((stsResult == PCAN_ERROR_OK) && (iBuffer == PCAN_CHANNEL_AVAILABLE))
        {
            list << QString("%1").arg(counter);
        }
        counter++;
    }

    m_userInterface->pcanDevicesComboBox->clear();
    m_userInterface->pcanDevicesComboBox->addItems(list);

}

/**
 * This slot function is called if the connection type has been changed.
 *
 * @param text
 *      The new text of the connecion type combo box.
 */
void SettingsDialog::conectionTypeChangesSlot(QString text)
{
    (void)text;
    initializeInterfaceTabs();
    updateSettings();
    emit conectionTypeChangesSignal();
}


/**
 * Slot function for the aardvark I2c/Spi scan button.
 */
void SettingsDialog::aardvarkI2cSpiScanButtonSlot(void)
{
    m_userInterface->aardvarkI2cSpiDevices->clear();
    m_userInterface->aardvarkI2cSpiDevices->appendPlainText(AardvarkI2cSpi::detectDevices());
}

/**
 * Slot function for the aardvark I2c/Spi free bus button.
 */
void SettingsDialog::aardvarkI2cSpiFreeBusButtonSlot(void)
{
    emit freeAardvarkI2cBusSignal();
}

/**
 * This slot function is called if the value of the script editor path line edit has been changed.
 * @param path
 *      The new path.
 */
void SettingsDialog::scriptEditorPathLineEditTextChangedSlot(QString path)
{
    (void)path;
    updateSettings();
    emit configHasToBeSavedSignal();
}

/**
 * Slot function for the search script editor button.
 */
void SettingsDialog::searchScriptEditorButtonPressedSlot(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open script editor"),
                                                    "",tr("Files (*)"));
    if(!fileName.isEmpty())
    {
        m_userInterface->scriptEditorPathLineEdit->setText(fileName);
        updateSettings();
        emit configHasToBeSavedSignal();
    }
}

/**
 * Shows the script window.
 */
void SettingsDialog::show(void)
{
    QWidget::show();

    fillSerialPortListBox();
}

/**
 * Is called if the input states of the aardvark I2c/Spi device have been changed.
 * @param states
 *      The new states (states contains AARDVARK_I2C_SPI_GPIO_COUNT elements).
 */
void SettingsDialog::aardvarkI2cSpiInputStatesChangedSlot(QVector<bool> states)
{
    for(int i = 0; (i < AARDVARK_I2C_SPI_GPIO_COUNT) && (i < states.size()); i++)
    {
        m_aardvarkI2cGpioGuiElements[i].inValue->setText(states[i] ? "1" : "0");
    }
}

/**
 * Sets all settings in the gui and the settings struct (m_currentSettings).
 * @param settings
 *      The new settings.
 * @param setTabIndex
 *      True if the tab index (settings dialog) shall set.
 */
void SettingsDialog::setAllSettingsSlot(Settings& settings, bool setTabIndex)
{
    showAllLocalIpAddresses();

    m_currentSettings = settings;

    m_userInterface->serialPortInfoListBox->setCurrentText(QString("%1").arg(settings.serialPort.name));

    //Baud Rate
    if((settings.serialPort.baudRate != 9600) && (settings.serialPort.baudRate != 19200) &&
            (settings.serialPort.baudRate != 38400) && (settings.serialPort.baudRate != 57600) && (settings.serialPort.baudRate != 115200))
    {
        m_userInterface->baudRateBox->setCurrentIndex(5);
    }
    m_userInterface->baudRateBox->setCurrentText(QString("%1").arg(settings.serialPort.baudRate));

    //Data bits
    m_userInterface->dataBitsBox->setCurrentText(QString("%1").arg(settings.serialPort.dataBits));

    //Stop bits
    m_userInterface->stopBitsBox->setCurrentText(QString("%1").arg(settings.serialPort.stopBits));

    //Parity
    m_userInterface->parityBox->setCurrentText(settings.serialPort.stringParity);

    //Flow control
    m_userInterface->flowControlBox->setCurrentText(settings.serialPort.stringFlowControl);

    //Console options
    m_userInterface->ShowReceivedCheckBox->setChecked(settings.showReceivedDataInConsole);
    m_userInterface->ShowSendInConsoleCheckBox->setChecked(settings.showSendDataInConsole);
    m_userInterface->PrintTimeStampCheckBox->setChecked(settings.generateTimeStampsInConsole);
    m_actionLockScrolling->setChecked(settings.lockScrollingInConsole);
    m_userInterface->ConsoleBufferLineEdit->setText(QString("%1").arg(settings.maxCharsInConsole));
    m_userInterface->consoleFontComboBox->setCurrentText(settings.stringConsoleFont);
    m_userInterface->consoleFontSizeComboBox->setCurrentText(settings.stringConsoleFontSize);
    m_userInterface->consoleI2cMetadata->setCurrentIndex((int)settings.i2cMetaInformationInConsole);
    m_userInterface->consoleShowAsciiCheckBox->setChecked(settings.showAsciiInConsole);
    m_userInterface->PrintTimeStampLineEdit->setText(QString("%1").arg(settings.timeStampIntervalConsole));
    m_userInterface->ConsoleUpdateIntervallLineEdit->setText(QString("%1").arg(settings.updateIntervalConsole));
    m_userInterface->consoleShowMixedCheckBox->setChecked(settings.showMixedConsole);
    checkMixedConsoleCheckbox();
    m_userInterface->consoleShowBinaryCheckBox->setChecked(settings.showBinaryConsole);
    m_userInterface->consoleShowCanMetaCheckBox->setChecked(settings.showCanMetaInformationInConsole);
    m_userInterface->showCanTabCheckBox->setChecked(settings.showCanTab);
    m_userInterface->consoleNewLineAfterNumberBytes->setText(QString("%1").arg(settings.consoleNewLineAfterBytes));
    m_userInterface->consoleNewLineAfterPause->setText(QString("%1").arg(settings.consoleNewLineAfterPause));
    m_userInterface->ConsoleTimestampFormat->setText(QString(settings.consoleTimestampFormat).replace("\n", "\\n"));


    m_userInterface->consoleTimestampAtByteCheckBox->setChecked(settings.consoleCreateTimestampAt);
    setDecimalComboBox(settings.consoleDecimalsType, m_userInterface->consoleDecimalsType);

    if(settings.consoleTimestampAt == 10)
    {
        m_userInterface->consoleTimestampAtByteComboBox->setCurrentIndex(0);
    }
    else if(settings.consoleTimestampAt == 13)
    {
        m_userInterface->consoleTimestampAtByteComboBox->setCurrentIndex(1);
    }
    else
    {//Custom
        m_userInterface->consoleTimestampAtByteComboBox->setCurrentIndex(2);
        m_userInterface->consoleTimestampAtByteComboBox->setCurrentText(QString("%1").arg((quint32)settings.consoleTimestampAt));
    }

    if(settings.consoleNewLineAt == 10)
    {
        m_userInterface->consoleNewLineAt->setCurrentIndex(0);
    }
    else if(settings.consoleNewLineAt == 13)
    {
        m_userInterface->consoleNewLineAt->setCurrentIndex(1);
    }
    else if(settings.consoleNewLineAt == 0xffff)
    {
        m_userInterface->consoleNewLineAt->setCurrentIndex(2);
    }
    else
    {//Custom
        m_userInterface->consoleNewLineAt->setCurrentIndex(3);
        m_userInterface->consoleNewLineAt->setCurrentText(QString("%1").arg((quint32)settings.consoleNewLineAt));
    }

    if (settings.consoleSendOnEnter == "\n")
    {//LF
        m_userInterface->consoleSendOnEnter->setCurrentIndex(0);
    }
    else if(settings.consoleSendOnEnter == "\r")
    {//CR
        m_userInterface->consoleSendOnEnter->setCurrentIndex(1);
    }
    else if(settings.consoleSendOnEnter == "\r\n")
    {//CR+LF
        m_userInterface->consoleSendOnEnter->setCurrentIndex(2);
    }
    else
    {//none
        m_userInterface->consoleSendOnEnter->setCurrentIndex(3);
    }

    if(settings.consoleReceiveColor.isEmpty()){settings.consoleReceiveColor = "000000";}
    if(settings.consoleSendColor.isEmpty()){settings.consoleSendColor = "7c0000";}
    if(settings.consoleBackgroundColor.isEmpty()){settings.consoleBackgroundColor = "efefef";}
    if(settings.consoleMessageAndTimestampColor.isEmpty()){settings.consoleMessageAndTimestampColor = "7c0000";}
    if(settings.consoleMixedAsciiColor.isEmpty()){settings.consoleMixedAsciiColor = "8faf9f";}
    if(settings.consoleMixedDecimalColor.isEmpty()){settings.consoleMixedDecimalColor = "f8f893";}
    if(settings.consoleMixedHexadecimalColor.isEmpty()){settings.consoleMixedHexadecimalColor = "6c9339";}
    if(settings.consoleMixedBinaryColor.isEmpty()){settings.consoleMixedBinaryColor = "bf9b76";}

    setButtonColorFromString(settings.consoleReceiveColor, m_userInterface->consoleReceiveColorButton);
    setButtonColorFromString(settings.consoleSendColor, m_userInterface->consoleSendColorButton);
    setButtonColorFromString(settings.consoleBackgroundColor, m_userInterface->consoleBackgroundColorButton);
    setButtonColorFromString(settings.consoleMessageAndTimestampColor, m_userInterface->consoleMessageColorButton);
    setButtonColorFromString(settings.consoleMixedAsciiColor, m_userInterface->btnColorAscii);
    setButtonColorFromString(settings.consoleMixedDecimalColor, m_userInterface->btnColorDec);
    setButtonColorFromString(settings.consoleMixedHexadecimalColor, m_userInterface->btnColorHex);
    setButtonColorFromString(settings.consoleMixedBinaryColor, m_userInterface->btnColorBin);

    //Log option
    m_userInterface->HtmlLogCheckBox->setChecked(settings.htmlLogFile);
    m_userInterface->logHtmlLineEdit->setText(settings.htmlLogfileName);
    m_userInterface->TextLogCheckBox->setChecked(settings.textLogFile);
    m_userInterface->logTextLineEdit->setText(settings.textLogfileName);
    m_userInterface->LogWriteReceivedCheckBox->setChecked(settings.writeReceivedDataInToLog);
    m_userInterface->LogWriteSendCheckBox->setChecked(settings.writeSendDataInToLog);
    m_userInterface->logWithTimeStampCheckBox->setChecked((settings.generateTimeStampsInLog));
    m_userInterface->logAppendTimestampCheckBox->setChecked((settings.appendTimestampAtLogFileName));
    m_userInterface->htmlLogFontComboBox->setCurrentText(settings.stringHtmlLogFont);
    m_userInterface->htmlLogFontSizeComboBox->setCurrentText(settings.stringHtmlLogFontSize);
    m_userInterface->consoleShowDecimalCheckBox->setChecked(settings.showDecimalInConsole);
    m_userInterface->consoleShowHexCheckBox->setChecked(settings.showHexInConsole);
    m_userInterface->logShowDecimalCheckBox->setChecked(settings.writeDecimalInToLog);
    m_userInterface->logShowHexCheckBox->setChecked(settings.writeHexInToLog);
    m_userInterface->logShowAsciiCheckBox->setChecked(settings.writeAsciiInToLog);
    m_userInterface->logShowBinaryCheckBox->setChecked(settings.writeBinaryInToLog);
    m_userInterface->logWithTimeStampLineEdit->setText(QString("%1").arg(settings.timeStampIntervalLog));
    m_userInterface->LogWriteCanMetaCheckBox->setChecked(settings.writeCanMetaInformationInToLog);
    m_userInterface->logNewLineAfterNumberBytes->setText(QString("%1").arg(settings.logNewLineAfterBytes));
    m_userInterface->logNewLineAfterPause->setText(QString("%1").arg(settings.logNewLineAfterPause));
    m_userInterface->LogTimestampFormat->setText(QString(settings.logTimestampFormat).replace("\n", "\\n"));
    m_userInterface->logTimestampAtByteCheckBox->setChecked(settings.logCreateTimestampAt);
    setDecimalComboBox(settings.logDecimalsType, m_userInterface->logDecimalsType);
    m_userInterface->logI2cMetadata->setCurrentIndex((int)settings.i2cMetaInformationInLog);

    if(settings.logTimestampAt == 10)
    {
        m_userInterface->logTimestampAtByteComboBox->setCurrentIndex(0);
    }
    else if(settings.logTimestampAt == 13)
    {
        m_userInterface->logTimestampAtByteComboBox->setCurrentIndex(1);
    }
    else
    {//Custom
        m_userInterface->logTimestampAtByteComboBox->setCurrentIndex(2);
        m_userInterface->logTimestampAtByteComboBox->setCurrentText(QString("%1").arg((quint32)settings.logTimestampAt));
    }

    if(settings.logNewLineAt == 10)
    {
        m_userInterface->logNewLineAt->setCurrentIndex(0);
    }
    else if(settings.logNewLineAt == 13)
    {
        m_userInterface->logNewLineAt->setCurrentIndex(1);
    }
    else if(settings.logNewLineAt == 0xffff)
    {
        m_userInterface->logNewLineAt->setCurrentIndex(2);
    }
    else
    {//Custom
        m_userInterface->logNewLineAt->setCurrentIndex(3);
        m_userInterface->logNewLineAt->setCurrentText(QString("%1").arg((quint32)settings.logNewLineAt));
    }

    m_userInterface->scriptEditorPathLineEdit->setText(settings.scriptEditorPath);
    m_userInterface->useExternalScriptEditorCheckBox->setChecked(settings.useExternalScriptEditor);
    m_userInterface->scriptEditorPathLineEdit->setEnabled(m_userInterface->useExternalScriptEditorCheckBox->isChecked());

    if(settings.connectionType == CONNECTION_TYPE_SERIAL_PORT)
    {
        m_userInterface->connectionTypeComboBox->setCurrentText("serial port");
    }
    else if(settings.connectionType == CONNECTION_TYPE_PCAN)
    {
        m_userInterface->connectionTypeComboBox->setCurrentText("pcan");
    }
    else if(settings.connectionType == CONNECTION_TYPE_AARDVARK)
    {
        m_userInterface->connectionTypeComboBox->setCurrentText("aardvark");
    }
    else
    {
        m_userInterface->connectionTypeComboBox->setCurrentText("socket");
    }

    m_userInterface->socketAddressLineEdit->setText(settings.socketSettings.destinationIpAddress);
    m_userInterface->socketPartnerPortLineEdit->setText(QString("%1").arg(settings.socketSettings.destinationPort));
    m_userInterface->socketOwnPortLineEdit->setText(QString("%1").arg(settings.socketSettings.ownPort));
    if(!settings.socketSettings.socketType.isEmpty())
    {
        m_userInterface->socketsTypeComboBox->setCurrentText(settings.socketSettings.socketType);
    }

    m_userInterface->proxyAddressLineEdit->setText(settings.socketSettings.proxyIpAddress);
    m_userInterface->proxyPortLineEdit->setText(QString("%1").arg(settings.socketSettings.proxyPort));
    m_userInterface->proxyUserNameLineEdit->setText(settings.socketSettings.proxyUserName);
    m_userInterface->proxyPasswordLineEdit->setText(settings.socketSettings.proxyPassword);
    if(settings.socketSettings.proxySettings == 1)
    {
        m_userInterface->useSystemProxyRadioButton->setChecked(true);
    }
    else if(settings.socketSettings.proxySettings == 2)
    {
        m_userInterface->useSpecificProxyRadioButton->setChecked(true);
    }
    else
    {
        m_userInterface->noProxyRadioButton->setChecked(true);

    }


    //Pcan settings
    m_userInterface->pcan5VoltComboBox->setCurrentText(settings.pcanInterface.powerSupply ? "on" : "off");
    m_userInterface->pcanBusOffResetComboBox->setCurrentText(settings.pcanInterface.busOffAutoReset ? "on" : "off");
    m_userInterface->pcanBaudratecomboBox->setCurrentText(convertPcanBaudrate(settings.pcanInterface.baudRate));
    if(settings.pcanInterface.channel > 0 && settings.pcanInterface.channel < 9)
    {
        m_userInterface->pcanDevicesComboBox->setCurrentText(QString("%1").arg(settings.pcanInterface.channel));
    }
    if(settings.pcanInterface.filterExtended){m_userInterface->pcanExtendedRadioButton->setChecked(true);}
    else {m_userInterface->pcanStandardRadioButton->setChecked(true);}
    setFilterRadioButtonPressedSlot();
    m_userInterface->pcanFilterFromLineEdit->blockSignals(true);
    m_userInterface->pcanFilterToLineEdit->blockSignals(true);
    if(!settings.pcanInterface.filterFrom.isEmpty()){m_userInterface->pcanFilterFromLineEdit->setText(settings.pcanInterface.filterFrom);}
    if(!settings.pcanInterface.filterTo.isEmpty()){m_userInterface->pcanFilterToLineEdit->setText(settings.pcanInterface.filterTo);}
    m_userInterface->pcanFilterFromLineEdit->blockSignals(false);
    m_userInterface->pcanFilterToLineEdit->blockSignals(false);

    if(settings.targetEndianess == LITTLE_ENDIAN_TARGET)
    {
        m_userInterface->endianessComboBox->setCurrentIndex(0);
    }
    else
    {
        m_userInterface->endianessComboBox->setCurrentIndex(1);
    }

    if(setTabIndex)
    {
        m_userInterface->tabWidget->setCurrentIndex(settings.settingsDialogTabIndex);
    }

    //Update settings
    m_userInterface->updateProxyAddress->setText(settings.updateSettings.proxyIpAddress);
    m_userInterface->updateProxyPort->setText(QString("%1").arg(settings.updateSettings.proxyPort));
    m_userInterface->updateProxyUserName->setText(settings.updateSettings.proxyUserName);
    m_userInterface->updateProxyPassword->setText(settings.updateSettings.proxyPassword);
    if(settings.updateSettings.proxySettings == 1)
    {
        m_userInterface->updateUseSystemProxy->setChecked(true);
    }
    else if(settings.updateSettings.proxySettings == 2)
    {
        m_userInterface->updateUseSpecificProxy->setChecked(true);
    }
    else
    {
        m_userInterface->updateNoProxy->setChecked(true);

    }

    //aardvark I2C/SPI settings
    m_userInterface->aardvarkI2cSpiPort->setText(QString("%1").arg(settings.aardvarkI2cSpi.devicePort));
    m_userInterface->aardvarkI2cSpiMode->setCurrentIndex((int)settings.aardvarkI2cSpi.deviceMode);
    m_userInterface->aardvarkI2cSpi5V->setCurrentText(settings.aardvarkI2cSpi.device5VIsOn ? "On" : "Off");
    m_userInterface->aardvarkI2cBaudrate->setText(QString("%1").arg(settings.aardvarkI2cSpi.i2cBaudrate));
    m_userInterface->aardvarkI2cSlaveAddress->setText(QString("%1").arg(settings.aardvarkI2cSpi.i2cSlaveAddress));
    m_userInterface->aardvarkI2cPullUp->setCurrentText(settings.aardvarkI2cSpi.i2cPullupsOn ? "On" : "Off");
    m_userInterface->aardvarkSpiPolarity->setCurrentIndex((int)settings.aardvarkI2cSpi.spiPolarity);
    m_userInterface->aardvarkSpiSSPolarity->setCurrentIndex((int)settings.aardvarkI2cSpi.spiSSPolarity);
    m_userInterface->aardvarkSpiBitorder->setCurrentIndex((int)settings.aardvarkI2cSpi.spiBitorder);
    m_userInterface->aardvarkSpiPhase->setCurrentIndex((int)settings.aardvarkI2cSpi.spiPhase);
    m_userInterface->aardvarkSpiBaudrate->setText(QString("%1").arg(settings.aardvarkI2cSpi.spiBaudrate));


    for(int i = 0; i < AARDVARK_I2C_SPI_GPIO_COUNT; i++)
    {
        if(settings.aardvarkI2cSpi.pinConfigs[i].isInput)
        {
            m_aardvarkI2cGpioGuiElements[i].mode->setCurrentText(settings.aardvarkI2cSpi.pinConfigs[i].withPullups ? "in pullup" : "in");
        }
        else
        {
            m_aardvarkI2cGpioGuiElements[i].mode->setCurrentText("out");
        }
        m_aardvarkI2cGpioGuiElements[i].outValue->setCurrentText(settings.aardvarkI2cSpi.pinConfigs[i].outValue ? "1" : "0");
    }

    updateSettings(true);

    initializeInterfaceTabs();
}

/**
 * Slot function for the activate html log check box.
 * @param activated
 *      True for activate.
 */
void SettingsDialog::htmLogActivatedSlot(bool activated)
{
    updateSettings();
    emit htmlLogActivatedSignal(activated);
}

/**
 * Slot function for the activate text log check box.
 * @param activated
 *      True for activate.
 */
void SettingsDialog::textLogActivatedSlot(bool activated)
{
    updateSettings();
    emit textLogActivatedSignal(activated);
}


/**
 * Returns the current settings.
 * @return
 *      The current settings.
 */
const Settings* SettingsDialog::settings() const
{
    return &m_currentSettings;
}

/**
 * This slot function is called if the user presses the lock scroll action in the main window.
 */
void SettingsDialog::lockScrollingSlot(void)
{
    updateSettings();
}

/**
 * This slot function is called if the selected com port has been changed.
 * @param text
 *      The new value.
 */
void SettingsDialog::comPortChangedSlot(QString text)
{
    for(auto list : getSerialPortsInfo())
    {
        if(text == list.at(0))
        {
            m_userInterface->descriptionLabel->setText(tr("Description: %1").arg(list.at(1)));
            m_userInterface->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.at(2)));
            m_userInterface->locationLabel->setText(tr("Location: %1").arg(list.at(3)));
            m_userInterface->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.at(4)));
            m_userInterface->pidLabel->setText(tr("Product Identifier: %1").arg(list.at(5)));
            break;
        }
        else
        {
            m_userInterface->descriptionLabel->setText(tr("Description: %1").arg(""));
            m_userInterface->manufacturerLabel->setText(tr("Manufacturer: %1").arg(""));
            m_userInterface->locationLabel->setText(tr("Location: %1").arg(""));
            m_userInterface->vidLabel->setText(tr("Vendor Identifier: %1").arg(""));
            m_userInterface->pidLabel->setText(tr("Product Identifier: %1").arg(""));
        }
    }
}

/**
 * The slot function for the close button.
 */
void SettingsDialog::closeButtonPressed()
{
    updateSettings();
    hide();
    emit configHasToBeSavedSignal();
}

/**
 * Slot function for the delete text log button.
 */
void SettingsDialog::deleteTextLogButtonPressedSlot(void)
{
    emit deleteLogFileSignal("text");
}

/**
 * Slot function for the search text log button.
 */
void SettingsDialog::searchTextLogButtonPressedSlot(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save text log file"),
                                                    "",tr("Files (*.txt)"));
    if(!fileName.isEmpty())
    {
        m_currentSettings.textLogfileName = fileName;
        m_userInterface->logTextLineEdit->setText(fileName);

        if(m_currentSettings.textLogFile)
        {
            emit textLogActivatedSignal(true);
        }
    }
}

/**
 * Slot function for the delete html log button.
 */
void SettingsDialog::deleteHtmlLogButtonPressedSlot(void)
{
    emit deleteLogFileSignal("html");
}

/**
 * Slot function for the search html log button.
 */
void SettingsDialog::searchHtmlLogButtonPressedSlot(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save html log file"),
                                                    "",tr("Files (*.html)"));
    if(!fileName.isEmpty())
    {
        m_currentSettings.htmlLogfileName = fileName;
        m_userInterface->logHtmlLineEdit->setText(fileName);

        if(m_currentSettings.htmlLogFile)
        {
            emit htmlLogActivatedSignal(true);
        }
    }
}


/**
 * This slot function is called if the selected baudrate has been changed.
 * If the user selects the custom value, then the combobox becomes editable and a int validator
 * is added (only the first time)
 * @param idx
 *      The index of the selected combobox item
 */
void SettingsDialog::checkCustomBaudRatePolicySlot(int idx)
{
    bool isCustomBaudRate = !m_userInterface->baudRateBox->itemData(idx).isValid();
    m_userInterface->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate)
    {
        m_userInterface->baudRateBox->clearEditText();
        QLineEdit *edit = m_userInterface->baudRateBox->lineEdit();

        if(edit->validator() == 0)
        {//the text edit has no validator

            //create a validator for the text edit
            edit->setValidator(new QIntValidator(0, MAX_BAUDRATE, edit));
        }
    }
}

/**
 * This slot function is called if the selected console time stamp at has been changed.
 * If the user selects the custom value, then the combobox becomes editable and a int validator
 * is added (only the first time).
 * @param idx
 *      The index of the selected combobox item
 */
void SettingsDialog::checkCustomConsoleTimestampAtSlot(int idx)
{
    bool isCustom= !m_userInterface->consoleTimestampAtByteComboBox->itemData(idx).isValid();
    m_userInterface->consoleTimestampAtByteComboBox->setEditable(isCustom);
    if (isCustom)
    {
        m_userInterface->consoleTimestampAtByteComboBox->clearEditText();
        QLineEdit *edit = m_userInterface->consoleTimestampAtByteComboBox->lineEdit();

        if(edit->validator() == 0)
        {//the text edit has no validator

            //create a validator for the text edit
            edit->setValidator(new QIntValidator(0, 255, edit));
        }
    }
}

/**
 * This slot function is called if the selected log time stamp at has been changed.
 * If the user selects the custom value, then the combobox becomes editable and a int validator
 * is added (only the first time).
 * @param idx
 *      The index of the selected combobox item
 */
void SettingsDialog::checkCustomLogTimestampAtSlot(int idx)
{
    bool isCustom= !m_userInterface->logTimestampAtByteComboBox->itemData(idx).isValid();
    m_userInterface->logTimestampAtByteComboBox->setEditable(isCustom);
    if (isCustom)
    {
        m_userInterface->logTimestampAtByteComboBox->clearEditText();
        QLineEdit *edit = m_userInterface->logTimestampAtByteComboBox->lineEdit();

        if(edit->validator() == 0)
        {//the text edit has no validator

            //create a validator for the text edit
            edit->setValidator(new QIntValidator(0, 255, edit));
        }
    }
}

/**
 * This slot function is called if the selected console new line has been changed.
 * If the user selects the custom value, then the combobox becomes editable and a int validator
 * is added (only the first time).
 * @param idx
 *      The index of the selected combobox item
 */
void SettingsDialog::checkCustomConsoleNewLineAtSlot(int idx)
{
    bool isCustom= !m_userInterface->consoleNewLineAt->itemData(idx).isValid();
    m_userInterface->consoleNewLineAt->setEditable(isCustom);
    if (isCustom)
    {
        m_userInterface->consoleNewLineAt->clearEditText();
        QLineEdit *edit = m_userInterface->consoleNewLineAt->lineEdit();

        if(edit->validator() == 0)
        {//the text edit has no validator

            //create a validator for the text edit
            edit->setValidator(new QIntValidator(0, 255, edit));
        }
    }
}

/**
 * This slot function is called if the selected log new line has been changed.
 * If the user selects the custom value, then the combobox becomes editable and a int validator
 * is added (only the first time)
 * @param idx
 *      The index of the selected combobox item
 */
void SettingsDialog::checkCustomLogNewLineAtSlot(int idx)
{
    bool isCustom= !m_userInterface->logNewLineAt->itemData(idx).isValid();
    m_userInterface->logNewLineAt->setEditable(isCustom);
    if (isCustom)
    {
        m_userInterface->logNewLineAt->clearEditText();
        QLineEdit *edit = m_userInterface->logNewLineAt->lineEdit();

        if(edit->validator() == 0)
        {//the text edit has no validator

            //create a validator for the text edit
            edit->setValidator(new QIntValidator(0, 255, edit));
        }
    }
}

/**
 * Sets m_interfaceSettingsCanBeChanged.
 * @param interfaceSettingsCanBeChanged
 *      The new value for m_interfaceSettingsCanBeChanged.
 */
void SettingsDialog::setInterfaceSettingsCanBeChanged(bool interfaceSettingsCanBeChanged)
{
    m_interfaceSettingsCanBeChanged = interfaceSettingsCanBeChanged;
    initializeInterfaceTabs();
}

/**
 * Initializes the aardvark I2C SPI interface tab.
 */
void SettingsDialog::initializeAardvarkIc2SpiTab(void)
{

    m_userInterface->aardvarkI2cPullUp->setEnabled(false);
    m_userInterface->aardvarkI2cFreeBus->setEnabled(false);

    m_userInterface->aardvarkSpiPolarity->setEnabled(false);
    m_userInterface->aardvarkSpiSSPolarity->setEnabled(false);
    m_userInterface->aardvarkSpiBitorder->setEnabled(false);
    m_userInterface->aardvarkSpiPhase->setEnabled(false);


    for(int i = 0; i < AARDVARK_I2C_SPI_GPIO_COUNT; i++)
    {
        m_aardvarkI2cGpioGuiElements[i].mode->setEnabled(false);
        m_aardvarkI2cGpioGuiElements[i].outValue->setEnabled(false);
    }

     if(m_interfaceSettingsCanBeChanged)
     {
         m_userInterface->aardvarkI2cSpiPort->setEnabled(true);
         m_userInterface->aardvarkI2cSpiMode->setEnabled(true);
         m_userInterface->aardvarkI2cSpiScan->setEnabled(true);
         m_userInterface->aardvarkI2cSpi5V->setEnabled(true);

         if((m_userInterface->aardvarkI2cSpiMode->currentText() == "I2C Master") ||
            (m_userInterface->aardvarkI2cSpiMode->currentText() == "I2C Slave"))
         {
             m_userInterface->aardvarkI2cBaudrate->setEnabled(true);
             m_userInterface->aardvarkI2cPullUp->setEnabled(true);
             m_userInterface->aardvarkI2cFreeBus->setEnabled(true);
             m_userInterface->aardvarkSpiBaudrate->setEnabled(false);

             if(m_userInterface->aardvarkI2cSpiMode->currentText() == "I2C Slave")
             {
                m_userInterface->aardvarkI2cSlaveAddress->setEnabled(true);
             }
             else
             {
                 m_userInterface->aardvarkI2cSlaveAddress->setEnabled(false);
             }
         }
         else if((m_userInterface->aardvarkI2cSpiMode->currentText() == "SPI Master") ||
                 (m_userInterface->aardvarkI2cSpiMode->currentText() == "SPI Slave"))
         {
             m_userInterface->aardvarkSpiPolarity->setEnabled(true);
             m_userInterface->aardvarkSpiSSPolarity->setEnabled(true);
             m_userInterface->aardvarkSpiBitorder->setEnabled(true);
             m_userInterface->aardvarkSpiPhase->setEnabled(true);
             m_userInterface->aardvarkSpiBaudrate->setEnabled(true);
             m_userInterface->aardvarkI2cBaudrate->setEnabled(false);
             m_userInterface->aardvarkI2cSlaveAddress->setEnabled(false);
         }
         else
         {
             m_userInterface->aardvarkSpiBaudrate->setEnabled(false);
             m_userInterface->aardvarkI2cBaudrate->setEnabled(false);
             m_userInterface->aardvarkI2cSlaveAddress->setEnabled(false);
         }

     }
     else
     {
         m_userInterface->aardvarkI2cSpiPort->setEnabled(false);
         m_userInterface->aardvarkI2cSpiMode->setEnabled(false);
         m_userInterface->aardvarkI2cSpiScan->setEnabled(false);
         m_userInterface->aardvarkI2cSpi5V->setEnabled(false);
         m_userInterface->aardvarkSpiBaudrate->setEnabled(false);
         m_userInterface->aardvarkI2cBaudrate->setEnabled(false);
         m_userInterface->aardvarkI2cSlaveAddress->setEnabled(false);
     }

     int startIndex = 0;
     int endIndex = 0;
     if((m_userInterface->aardvarkI2cSpiMode->currentText() == "I2C Master") ||
        (m_userInterface->aardvarkI2cSpiMode->currentText() == "I2C Slave"))
     {
         startIndex = 2;
         endIndex = AARDVARK_I2C_SPI_GPIO_COUNT - 1;
     }
     else if((m_userInterface->aardvarkI2cSpiMode->currentText() == "SPI Master") ||
             (m_userInterface->aardvarkI2cSpiMode->currentText() == "SPI Slave"))
     {
         startIndex = 0;
         endIndex = 1;
     }
     else
     {
         startIndex = 0;
         endIndex = AARDVARK_I2C_SPI_GPIO_COUNT - 1;
     }

     for(int i = startIndex; i <= endIndex; i++)
     {
         m_aardvarkI2cGpioGuiElements[i].mode->setEnabled(true);
         m_aardvarkI2cGpioGuiElements[i].outValue->setEnabled((m_aardvarkI2cGpioGuiElements[i].mode->currentText() == "out") ? true : false);
     }
}

/**
 * Initializes the pcan interface tab.
 */
void SettingsDialog::initializePcanTab(void)
{
    if(m_interfaceSettingsCanBeChanged)
    {

        m_userInterface->pcan5VoltComboBox->setEnabled(true);
        m_userInterface->pcanBaudratecomboBox->setEnabled(true);
        m_userInterface->pcanBusOffResetComboBox->setEnabled(true);
        m_userInterface->pcanDetectPushButton->setEnabled(true);
        m_userInterface->pcanDevicesComboBox->setEnabled(true);
        m_userInterface->pcanExtendedRadioButton->setEnabled(true);
        m_userInterface->pcanStandardRadioButton->setEnabled(true);
        m_userInterface->pcanFilterFromLineEdit->setEnabled(true);
        m_userInterface->pcanFilterToLineEdit->setEnabled(true);

    }
    else
    {
        m_userInterface->pcan5VoltComboBox->setEnabled(false);
        m_userInterface->pcanBaudratecomboBox->setEnabled(false);
        m_userInterface->pcanBusOffResetComboBox->setEnabled(false);
        m_userInterface->pcanDetectPushButton->setEnabled(false);
        m_userInterface->pcanDevicesComboBox->setEnabled(false);
        m_userInterface->pcanExtendedRadioButton->setEnabled(false);
        m_userInterface->pcanStandardRadioButton->setEnabled(false);
        m_userInterface->pcanFilterFromLineEdit->setEnabled(false);
        m_userInterface->pcanFilterToLineEdit->setEnabled(false);
    }
}

/**
 * Initializes the update tab.
 */
void SettingsDialog::initializeUpdateTab(void)
{
    //Update proxy settings.
    m_userInterface->updateNoProxy->setEnabled(true);
    m_userInterface->updateUseSystemProxy->setEnabled(true);
    m_userInterface->updateUseSpecificProxy->setEnabled(true);
    if(m_userInterface->updateUseSpecificProxy->isChecked())
    {
        m_userInterface->updateProxyAddress->setEnabled(true);
        m_userInterface->updateProxyPort->setEnabled(true);
        m_userInterface->updateProxyUserName->setEnabled(true);
        m_userInterface->updateProxyPassword->setEnabled(true);
    }
    else
    {
        m_userInterface->updateProxyAddress->setEnabled(false);
        m_userInterface->updateProxyPort->setEnabled(false);

        if(m_userInterface->updateUseSystemProxy->isChecked())
        {
            m_userInterface->updateProxyUserName->setEnabled(true);
            m_userInterface->updateProxyPassword->setEnabled(true);
        }
        else
        {
            m_userInterface->updateProxyUserName->setEnabled(false);
            m_userInterface->updateProxyPassword->setEnabled(false);
        }
    }
}

/**
 * Initializes the sockets interface tab.
 */
void SettingsDialog::initializeSocketsTab(void)
{
    if(m_interfaceSettingsCanBeChanged)
    {
        m_userInterface->connectionTypeComboBox->setEnabled(true);
        m_userInterface->socketsTypeComboBox->setEnabled(true);
        m_userInterface->socketAddressLineEdit->setEnabled(true);
        m_userInterface->socketOwnPortLineEdit->setEnabled(true);


        if(m_userInterface->socketsTypeComboBox->currentText() == "TCP client")
        {
            m_userInterface->socketOwnPortLineEdit->setEnabled(false);
        }

        if(m_userInterface->socketsTypeComboBox->currentText() == "TCP server")
        {
            m_userInterface->socketPartnerPortLineEdit->setEnabled(false);
            m_userInterface->socketAddressLineEdit->setEnabled(false);

        }
        else
        {
            m_userInterface->socketPartnerPortLineEdit->setEnabled(true);
        }

        //Socket proxy settings.
        m_userInterface->noProxyRadioButton->setEnabled(true);
        m_userInterface->useSystemProxyRadioButton->setEnabled(true);
        m_userInterface->useSpecificProxyRadioButton->setEnabled(true);
        if(m_userInterface->useSpecificProxyRadioButton->isChecked())
        {
            m_userInterface->proxyAddressLineEdit->setEnabled(true);
            m_userInterface->proxyPortLineEdit->setEnabled(true);
            m_userInterface->proxyUserNameLineEdit->setEnabled(true);
            m_userInterface->proxyPasswordLineEdit->setEnabled(true);
        }
        else
        {
            m_userInterface->proxyAddressLineEdit->setEnabled(false);
            m_userInterface->proxyPortLineEdit->setEnabled(false);

            if(m_userInterface->useSystemProxyRadioButton->isChecked())
            {
                m_userInterface->proxyUserNameLineEdit->setEnabled(true);
                m_userInterface->proxyPasswordLineEdit->setEnabled(true);
            }
            else
            {
                m_userInterface->proxyUserNameLineEdit->setEnabled(false);
                m_userInterface->proxyPasswordLineEdit->setEnabled(false);
            }
        }

    }
    else
    {
        m_userInterface->connectionTypeComboBox->setEnabled(false);
        m_userInterface->socketsTypeComboBox->setEnabled(false);
        m_userInterface->socketPartnerPortLineEdit->setEnabled(false);
        m_userInterface->socketAddressLineEdit->setEnabled(false);
        m_userInterface->socketOwnPortLineEdit->setEnabled(false);

        m_userInterface->noProxyRadioButton->setEnabled(false);
        m_userInterface->useSystemProxyRadioButton->setEnabled(false);
        m_userInterface->useSpecificProxyRadioButton->setEnabled(false);
        m_userInterface->proxyAddressLineEdit->setEnabled(false);
        m_userInterface->proxyPortLineEdit->setEnabled(false);
        m_userInterface->proxyUserNameLineEdit->setEnabled(false);
        m_userInterface->proxyPasswordLineEdit->setEnabled(false);

    }
}

/**
 * Initializes the interface tabs.
 */
void SettingsDialog::initializeInterfaceTabs(void)
{

    initializeSocketsTab();
    initializePcanTab();
    initializeUpdateTab();
    initializeAardvarkIc2SpiTab();
}

/**
 * Adds all available serial ports to the serial port list box.
 */
void SettingsDialog::fillSerialPortListBox(void)
{
    m_userInterface->serialPortInfoListBox->blockSignals(true);
    qDebug() << "mouse";

    QString currentText = m_userInterface->serialPortInfoListBox->currentText();
    bool savedTextIsAvailable = false;
    //read all serial port informations
    QVector<QStringList> list = getSerialPortsInfo();
    m_userInterface->serialPortInfoListBox->clear();
    for(auto entry : list)
    {
        m_userInterface->serialPortInfoListBox->addItem(entry.at(0));

        if(entry.at(0) == currentText)
        {
            savedTextIsAvailable = true;
        }
    }

    if(savedTextIsAvailable)
    {
        m_userInterface->serialPortInfoListBox->setCurrentText(currentText);
    }

    m_userInterface->serialPortInfoListBox->blockSignals(false);
}

/**
 * In this event filter function the mouse pressed event for serialPortInfoListBox
 * is intercepted and a scan for available serial port is done.
 * The previous elements of serialPortInfoListBox are replaced by the result of the scan.
 * After this the mouse event is passed to the parent of serialPortInfoListBox.
 *
 * @param obj
 *      The current object.
 * @param event
 *      The current event.
 * @return
 *  True if the event should be filtered out.
 */
bool SettingsDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_userInterface->serialPortInfoListBox)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            fillSerialPortListBox();
        }

    }
    return QDialog::eventFilter(obj, event);
}

/**
 * This slot function is called if a text from a gui element has been changed.
 * It updates the gui elements and the settings struct (m_currentSettings).
 * @param text
 *      The new text.
 */
void SettingsDialog::textFromGuiElementChangedSlot(QString text)
{
    (void) text;
    initializeInterfaceTabs();
    updateSettings();
    emit configHasToBeSavedSignal();


    QObject* obj = sender();
    for(int i = 0; i < AARDVARK_I2C_SPI_GPIO_COUNT; i++)
    {
        if(m_aardvarkI2cGpioGuiElements[i].mode == obj)
        {
            emit pinConfigChangedSignal(m_currentSettings.aardvarkI2cSpi);
            break;
        }

        if(m_aardvarkI2cGpioGuiElements[i].outValue == obj)
        {
            emit outputValueChangedSignal(m_currentSettings.aardvarkI2cSpi);
            break;
        }
    }
}

/**
 * Is called if the user clickes a socket proxy radio button.
 */
void SettingsDialog::socketProxyRadioButtonClickedSlot(void)
{
    initializeInterfaceTabs();
    updateSettings();
    emit configHasToBeSavedSignal();
}

/**
 * Is called if the user clickes a update proxy radio button.
 */
void SettingsDialog::updateProxyRadioButtonClickedSlot(void)
{
    initializeInterfaceTabs();
    updateSettings();
    emit configHasToBeSavedSignal();
}


/**
 * Checks if the mixed console chekcbox can be activated.
 */
void SettingsDialog::checkMixedConsoleCheckbox()
{
    if(!m_userInterface->consoleShowAsciiCheckBox->isChecked() &&
       !m_userInterface->consoleShowHexCheckBox->isChecked() &&
       !m_userInterface->consoleShowBinaryCheckBox->isChecked() &&
       !m_userInterface->consoleShowDecimalCheckBox->isChecked())
    {
        m_userInterface->consoleShowMixedCheckBox->setChecked(false);
        m_userInterface->consoleShowMixedCheckBox->setEnabled(false);
    }
    else
    {
        m_userInterface->consoleShowMixedCheckBox->setEnabled(true);
    }
}

/**
 * This slot function is called if a selection from a check box has been changed.
 * It updates the settings struct (m_currentSettings).
 * @param state
 *      The new state.
 */
void SettingsDialog::stateFromCheckboxChangedSlot(int state)
{
    (void) state;

    m_userInterface->scriptEditorPathLineEdit->setEnabled(m_userInterface->useExternalScriptEditorCheckBox->isChecked());

    updateSettings();
    emit configHasToBeSavedSignal();

    checkMixedConsoleCheckbox();
}

/**
 * Fills the gui elements which correspond to the serial port.
 */
void SettingsDialog::fillSerialPortParameters()
{
    // fill baud rate
    m_userInterface->baudRateBox->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    m_userInterface->baudRateBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    m_userInterface->baudRateBox->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    m_userInterface->baudRateBox->addItem(QLatin1String("57600"), QSerialPort::Baud57600);
    m_userInterface->baudRateBox->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    m_userInterface->baudRateBox->addItem(QLatin1String("custom"));
    m_userInterface->baudRateBox->setCurrentIndex(4);

    // fill data bits
    m_userInterface->dataBitsBox->addItem(QLatin1String("5"), QSerialPort::Data5);
    m_userInterface->dataBitsBox->addItem(QLatin1String("6"), QSerialPort::Data6);
    m_userInterface->dataBitsBox->addItem(QLatin1String("7"), QSerialPort::Data7);
    m_userInterface->dataBitsBox->addItem(QLatin1String("8"), QSerialPort::Data8);
    m_userInterface->dataBitsBox->setCurrentIndex(3);

    // fill parity
    m_userInterface->parityBox->addItem(QLatin1String("None"), QSerialPort::NoParity);
    m_userInterface->parityBox->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    m_userInterface->parityBox->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    m_userInterface->parityBox->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    m_userInterface->parityBox->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);

    // fill stop bits
    m_userInterface->stopBitsBox->addItem(QLatin1String("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_userInterface->stopBitsBox->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_userInterface->stopBitsBox->addItem(QLatin1String("2"), QSerialPort::TwoStop);

    // fill flow control
    m_userInterface->flowControlBox->addItem(QLatin1String("None"), QSerialPort::NoFlowControl);
    m_userInterface->flowControlBox->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    m_userInterface->flowControlBox->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);
}

/**
 * Reads the information from all available serial port.
 * @return
 *      The port informations.
 */
QVector<QStringList> SettingsDialog::getSerialPortsInfo()
{
    QVector<QStringList> listVector;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QStringList list;
        list << info.portName()
             << info.description()
             << info.manufacturer()
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString())
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());

        listVector.append(list);
    }
    return listVector;
}

/**
 * Returns the text color from a button
 * @param button
 *      The color button.
 * @return
 *  The color
 */
QString SettingsDialog::getColorStringFromButton(QToolButton* button)
{
    QString colorString;

    QColor color =  button->palette().color(QPalette::Button);

    int r;
    int g;
    int b;
    color.getRgb(&r, &g, &b);


    QString tmp = QString::number(r,16);
    if(tmp.size() == 1)
    {
        tmp = "0" + tmp;
    }
    colorString = tmp;

    tmp = QString::number(g,16);
    if(tmp.size() == 1)
    {
        tmp = "0" + tmp;
    }
    colorString += tmp;

    tmp = QString::number(b,16);
    if(tmp.size() == 1)
    {
        tmp = "0" + tmp;
    }
    colorString += tmp;

    return colorString;
}

/**
 * Sets the text color of a button.
 * @param colorString
 *      The text color.
 * @param button
 *      The button.
 */
void SettingsDialog::setButtonColorFromString(QString colorString, QToolButton* button)
{
    bool isOk;

    int r = colorString.mid(0, 2).toUInt(&isOk, 16);
    int g = colorString.mid(2, 2).toUInt(&isOk, 16);
    int b = colorString.mid(4, 2).toUInt(&isOk, 16);

    QColor color(r,g,b);
    button->setStyleSheet(QString("background-color: %1;").arg(color.name()));
}

/**
 * Updates a decimal type.
 * @param type
 *      The new decimal type.
 * @param typeBox
 *      The decimal type combo box.
 */
void SettingsDialog::updatesDecimalsTypes(DecimalType* type, QComboBox* typeBox)
{
    if(typeBox->currentText() == "uint8")
    {
        *type = DECIMAL_TYPE_UINT8;
    }
    else if(typeBox->currentText() == "int8")
    {
        *type = DECIMAL_TYPE_INT8;
    }
    else if(typeBox->currentText() == "uint16")
    {
        *type = DECIMAL_TYPE_UINT16;
    }
    else if(typeBox->currentText() == "int16")
    {
        *type = DECIMAL_TYPE_INT16;
    }
    else if(typeBox->currentText() == "uint32")
    {
        *type = DECIMAL_TYPE_UINT32;
    }
    else if(typeBox->currentText() == "int32")
    {
        *type = DECIMAL_TYPE_INT32;
    }
    else
    {
        *type = DECIMAL_TYPE_UINT8;
    }
}


/**
 * Sets a decimal combo box.
 *
 * @param type
 *      The new decimal type.
 * @param typeBox
 *      The decimal type combo box.
 */
void SettingsDialog::setDecimalComboBox(DecimalType type, QComboBox* typeBox)
{
    if(type == DECIMAL_TYPE_UINT8)
    {
        typeBox->setCurrentText("uint8");
    }
    else if(type == DECIMAL_TYPE_INT8)
    {
        typeBox->setCurrentText("int8");
    }
    else if(type == DECIMAL_TYPE_UINT16)
    {
        typeBox->setCurrentText("uint16");
    }
    else if(type == DECIMAL_TYPE_INT16)
    {
        typeBox->setCurrentText("int16");
    }
    else if(type == DECIMAL_TYPE_UINT32)
    {
        typeBox->setCurrentText("uint32");
    }
    else if(type == DECIMAL_TYPE_INT32)
    {
        typeBox->setCurrentText("int32");
    }
    else
    {
        typeBox->setCurrentText("uint8");
    }
}

/**
 * Updates the settings struct (m_currentSettings).
 */
void SettingsDialog::updateSettings(bool forceUpdate)
{
    if(signalsBlocked() && !forceUpdate)
    {
        return;
    }
    m_currentSettings.serialPort.name = m_userInterface->serialPortInfoListBox->currentText();

    // Baud Rate
    if (m_userInterface->baudRateBox->currentIndex() == 5)
    {
        // custom baud rate
        m_currentSettings.serialPort.baudRate = m_userInterface->baudRateBox->currentText().toInt();
    }
    else
    {
        // standard baud rate
        m_currentSettings.serialPort.baudRate = static_cast<QSerialPort::BaudRate>(
                    m_userInterface->baudRateBox->itemData(m_userInterface->baudRateBox->currentIndex()).toInt());
    }
    m_currentSettings.serialPort.stringBaudRate = QString::number(m_currentSettings.serialPort.baudRate);

    // Data bits
    m_currentSettings.serialPort.dataBits = static_cast<QSerialPort::DataBits>(
                m_userInterface->dataBitsBox->itemData(m_userInterface->dataBitsBox->currentIndex()).toInt());
    m_currentSettings.serialPort.stringDataBits = m_userInterface->dataBitsBox->currentText();

    // Parity
    m_currentSettings.serialPort.parity = static_cast<QSerialPort::Parity>(
                m_userInterface->parityBox->itemData(m_userInterface->parityBox->currentIndex()).toInt());
    m_currentSettings.serialPort.stringParity = m_userInterface->parityBox->currentText();

    // Stop bits
    m_currentSettings.serialPort.stopBits = static_cast<QSerialPort::StopBits>(
                m_userInterface->stopBitsBox->itemData(m_userInterface->stopBitsBox->currentIndex()).toInt());
    m_currentSettings.serialPort.stringStopBits = m_userInterface->stopBitsBox->currentText();

    // Flow control
    m_currentSettings.serialPort.flowControl = static_cast<QSerialPort::FlowControl>(
                m_userInterface->flowControlBox->itemData(m_userInterface->flowControlBox->currentIndex()).toInt());
    m_currentSettings.serialPort.stringFlowControl = m_userInterface->flowControlBox->currentText();

    // console options
    m_currentSettings.showReceivedDataInConsole = m_userInterface->ShowReceivedCheckBox->isChecked();
    m_currentSettings.showSendDataInConsole = m_userInterface->ShowSendInConsoleCheckBox->isChecked();
    m_currentSettings.generateTimeStampsInConsole = m_userInterface->PrintTimeStampCheckBox->isChecked();
    m_currentSettings.maxCharsInConsole = m_userInterface->ConsoleBufferLineEdit->text().toInt();
    m_currentSettings.lockScrollingInConsole = m_actionLockScrolling->isChecked();
    m_currentSettings.stringConsoleFont = m_userInterface->consoleFontComboBox->currentText();
    m_currentSettings.stringConsoleFontSize = m_userInterface->consoleFontSizeComboBox->currentText();
    m_currentSettings.i2cMetaInformationInConsole = (I2cMetadata)m_userInterface->consoleI2cMetadata->currentIndex();
    m_currentSettings.showDecimalInConsole = m_userInterface->consoleShowDecimalCheckBox->isChecked();
    m_currentSettings.showHexInConsole = m_userInterface->consoleShowHexCheckBox->isChecked();
    m_currentSettings.showAsciiInConsole = m_userInterface->consoleShowAsciiCheckBox->isChecked();
    m_currentSettings.timeStampIntervalConsole = m_userInterface->PrintTimeStampLineEdit->text().toInt();
    m_currentSettings.updateIntervalConsole = m_userInterface->ConsoleUpdateIntervallLineEdit->text().toUInt();
    m_userInterface->ConsoleUpdateIntervallLineEdit->setText(QString("%1").arg(m_currentSettings.updateIntervalConsole));
    m_currentSettings.showMixedConsole = m_userInterface->consoleShowMixedCheckBox->isChecked();
    m_currentSettings.showBinaryConsole = m_userInterface->consoleShowBinaryCheckBox->isChecked();
    m_currentSettings.showCanMetaInformationInConsole = m_userInterface->consoleShowCanMetaCheckBox->isChecked();
    m_currentSettings.showCanTab= m_userInterface->showCanTabCheckBox->isChecked();
    m_currentSettings.consoleReceiveColor= getColorStringFromButton(m_userInterface->consoleReceiveColorButton);
    m_currentSettings.consoleSendColor= getColorStringFromButton(m_userInterface->consoleSendColorButton);
    m_currentSettings.consoleBackgroundColor= getColorStringFromButton(m_userInterface->consoleBackgroundColorButton);
    m_currentSettings.consoleMessageAndTimestampColor= getColorStringFromButton(m_userInterface->consoleMessageColorButton);
    m_currentSettings.consoleMixedAsciiColor = getColorStringFromButton(m_userInterface->btnColorAscii);
    m_currentSettings.consoleMixedDecimalColor = getColorStringFromButton(m_userInterface->btnColorDec);
    m_currentSettings.consoleMixedHexadecimalColor= getColorStringFromButton(m_userInterface->btnColorHex);
    m_currentSettings.consoleMixedBinaryColor= getColorStringFromButton(m_userInterface->btnColorBin);
    m_currentSettings.consoleNewLineAfterBytes = m_userInterface->consoleNewLineAfterNumberBytes->text().toUInt();
    m_currentSettings.consoleNewLineAfterPause = m_userInterface->consoleNewLineAfterPause->text().toUInt();
    m_currentSettings.consoleSendOnEnter = m_userInterface->consoleSendOnEnter->itemData(m_userInterface->consoleSendOnEnter->currentIndex()).toString();
    m_currentSettings.consoleTimestampFormat = m_userInterface->ConsoleTimestampFormat->text();
    m_currentSettings.consoleTimestampFormat.replace("\\n", "\n");
    m_currentSettings.consoleCreateTimestampAt= m_userInterface->consoleTimestampAtByteCheckBox->isChecked();
    updatesDecimalsTypes(&m_currentSettings.consoleDecimalsType, m_userInterface->consoleDecimalsType);


    //Console time stamp at byte.
    if (m_userInterface->consoleTimestampAtByteComboBox->currentIndex() == 2)
    {
        //custom
        m_currentSettings.consoleTimestampAt =  (quint16)m_userInterface->consoleTimestampAtByteComboBox->currentText().toUInt();
    }
    else
    {
        //standard
        m_currentSettings.consoleTimestampAt =  (quint16)m_userInterface->consoleTimestampAtByteComboBox->itemData(m_userInterface->consoleTimestampAtByteComboBox->currentIndex()).toUInt();
    }


    //New console line at.
    if (m_userInterface->consoleNewLineAt->currentIndex() == 3)
    {
        //custom
        m_currentSettings.consoleNewLineAt =  (quint16)m_userInterface->consoleNewLineAt->currentText().toUInt();
    }
    else
    {
        //standard
        m_currentSettings.consoleNewLineAt =  (quint16)m_userInterface->consoleNewLineAt->itemData(m_userInterface->consoleNewLineAt->currentIndex()).toUInt();
    }


    // log option
    m_currentSettings.htmlLogFile = m_userInterface->HtmlLogCheckBox->isChecked();
    m_currentSettings.textLogFile = m_userInterface->TextLogCheckBox->isChecked();
    m_currentSettings.writeReceivedDataInToLog = m_userInterface->LogWriteReceivedCheckBox->isChecked();
    m_currentSettings.writeSendDataInToLog = m_userInterface->LogWriteSendCheckBox->isChecked();
    m_currentSettings.generateTimeStampsInLog = m_userInterface->logWithTimeStampCheckBox->isChecked();
    m_currentSettings.appendTimestampAtLogFileName = m_userInterface->logAppendTimestampCheckBox->isChecked();
    m_currentSettings.stringHtmlLogFont = m_userInterface->htmlLogFontComboBox->currentText();
    m_currentSettings.stringHtmlLogFontSize = m_userInterface->htmlLogFontSizeComboBox->currentText();
    m_currentSettings.writeDecimalInToLog = m_userInterface->logShowDecimalCheckBox->isChecked();
    m_currentSettings.writeHexInToLog = m_userInterface->logShowHexCheckBox->isChecked();
    m_currentSettings.writeAsciiInToLog = m_userInterface->logShowAsciiCheckBox->isChecked();
    m_currentSettings.writeBinaryInToLog = m_userInterface->logShowBinaryCheckBox->isChecked();
    m_currentSettings.timeStampIntervalLog = m_userInterface->logWithTimeStampLineEdit->text().toInt();
    m_currentSettings.writeCanMetaInformationInToLog = m_userInterface->LogWriteCanMetaCheckBox->isChecked();
    m_currentSettings.logNewLineAfterBytes = m_userInterface->logNewLineAfterNumberBytes->text().toUInt();
    m_currentSettings.logNewLineAfterPause = m_userInterface->logNewLineAfterPause->text().toUInt();
    m_currentSettings.logTimestampFormat = m_userInterface->LogTimestampFormat->text();
    m_currentSettings.logTimestampFormat.replace("\\n", "\n");
    m_currentSettings.logCreateTimestampAt= m_userInterface->logTimestampAtByteCheckBox->isChecked();
    updatesDecimalsTypes(&m_currentSettings.logDecimalsType, m_userInterface->logDecimalsType);
    m_currentSettings.i2cMetaInformationInLog = (I2cMetadata)m_userInterface->logI2cMetadata->currentIndex();

    //Log time satmp at byte.
    if (m_userInterface->logTimestampAtByteComboBox->currentIndex() == 2)
    {
        //custom
        m_currentSettings.logTimestampAt =  (quint16)m_userInterface->logTimestampAtByteComboBox->currentText().toUInt();
    }
    else
    {
        //standard
        m_currentSettings.logTimestampAt =  (quint16)m_userInterface->logTimestampAtByteComboBox->itemData(m_userInterface->logTimestampAtByteComboBox->currentIndex()).toUInt();
    }

    //New log line at.
    if (m_userInterface->logNewLineAt->currentIndex() == 3)
    {
        //custom
        m_currentSettings.logNewLineAt =  (quint16)m_userInterface->logNewLineAt->currentText().toUInt();
    }
    else
    {
        //standard
        m_currentSettings.logNewLineAt =  (quint16)m_userInterface->logNewLineAt->itemData(m_userInterface->logNewLineAt->currentIndex()).toUInt();
    }

    m_currentSettings.scriptEditorPath = m_userInterface->scriptEditorPathLineEdit->text();
    m_currentSettings.useExternalScriptEditor = m_userInterface->useExternalScriptEditorCheckBox->isChecked();

    if(m_userInterface->connectionTypeComboBox->currentText() == "socket")
    {
        if(m_userInterface->socketsTypeComboBox->currentText() == "TCP client")
        {
            m_currentSettings.connectionType = CONNECTION_TYPE_TCP_CLIENT;
        }
        else if(m_userInterface->socketsTypeComboBox->currentText() == "TCP server")
        {
            m_currentSettings.connectionType = CONNECTION_TYPE_TCP_SERVER;
        }
        else
        {//UDP socket
            m_currentSettings.connectionType = CONNECTION_TYPE_UDP_SOCKET;
        }
    }
    else if(m_userInterface->connectionTypeComboBox->currentText() == "serial port")
    {
        m_currentSettings.connectionType = CONNECTION_TYPE_SERIAL_PORT;
    }
    else if(m_userInterface->connectionTypeComboBox->currentText() == "pcan")
    {
        m_currentSettings.connectionType = CONNECTION_TYPE_PCAN;
    }
    else
    {
        m_currentSettings.connectionType = CONNECTION_TYPE_AARDVARK;
    }


    m_currentSettings.socketSettings.destinationIpAddress = m_userInterface->socketAddressLineEdit->text();
    m_currentSettings.socketSettings.destinationPort = m_userInterface->socketPartnerPortLineEdit->text().toUInt();
    m_currentSettings.socketSettings.ownPort = m_userInterface->socketOwnPortLineEdit->text().toUInt();
    m_currentSettings.socketSettings.socketType = m_userInterface->socketsTypeComboBox->currentText();

    m_currentSettings.socketSettings.proxyIpAddress = m_userInterface->proxyAddressLineEdit->text();
    m_currentSettings.socketSettings.proxyPort = m_userInterface->proxyPortLineEdit->text().toUInt();
    m_currentSettings.socketSettings.proxyUserName = m_userInterface->proxyUserNameLineEdit->text();
    m_currentSettings.socketSettings.proxyPassword = m_userInterface->proxyPasswordLineEdit->text();

    if(m_userInterface->useSystemProxyRadioButton->isChecked())
    {
        m_currentSettings.socketSettings.proxySettings = 1;

    }
    else if(m_userInterface->useSpecificProxyRadioButton->isChecked())
    {
        m_currentSettings.socketSettings.proxySettings = 2;

    }
    else
    {
        m_currentSettings.socketSettings.proxySettings = 0;
    }


    //pcan settings
    m_currentSettings.pcanInterface.baudRate = getPcanBaudrate();
    m_currentSettings.pcanInterface.busOffAutoReset = (m_userInterface->pcanBusOffResetComboBox->currentText() == "on") ? true : false;
    m_currentSettings.pcanInterface.powerSupply = (m_userInterface->pcan5VoltComboBox->currentText() == "on") ? true : false;
    m_currentSettings.pcanInterface.channel = m_userInterface->pcanDevicesComboBox->currentText().toUInt();
    m_currentSettings.pcanInterface.filterExtended = m_userInterface->pcanExtendedRadioButton->isChecked();
    m_currentSettings.pcanInterface.filterFrom = m_userInterface->pcanFilterFromLineEdit->text();
    m_currentSettings.pcanInterface.filterTo = m_userInterface->pcanFilterToLineEdit->text();



    m_currentSettings.targetEndianess = (m_userInterface->endianessComboBox->currentIndex() == 0) ? LITTLE_ENDIAN_TARGET : BIG_ENDIAN_TARGET;

    m_currentSettings.settingsDialogTabIndex = m_userInterface->tabWidget->currentIndex();


    //update settings
    m_currentSettings.updateSettings.proxyIpAddress = m_userInterface->updateProxyAddress->text();
    m_currentSettings.updateSettings.proxyPort = m_userInterface->updateProxyPort->text().toUInt();
    m_currentSettings.updateSettings.proxyUserName = m_userInterface->updateProxyUserName->text();
    m_currentSettings.updateSettings.proxyPassword = m_userInterface->updateProxyPassword->text();

    if(m_userInterface->updateUseSystemProxy->isChecked())
    {
        m_currentSettings.updateSettings.proxySettings = 1;

    }
    else if(m_userInterface->updateUseSpecificProxy->isChecked())
    {
        m_currentSettings.updateSettings.proxySettings = 2;

    }
    else
    {
        m_currentSettings.updateSettings.proxySettings = 0;
    }

    //aardvark I2C/SPI settings
    m_currentSettings.aardvarkI2cSpi.devicePort = m_userInterface->aardvarkI2cSpiPort->text().toUInt();
    m_currentSettings.aardvarkI2cSpi.deviceMode = (AardvarkI2cSpiDeviceMode)m_userInterface->aardvarkI2cSpiMode->currentIndex();
    m_currentSettings.aardvarkI2cSpi.device5VIsOn = (m_userInterface->aardvarkI2cSpi5V->currentText() == "On") ? true : false;
    m_currentSettings.aardvarkI2cSpi.i2cBaudrate = m_userInterface->aardvarkI2cBaudrate->text().toUInt();
    m_currentSettings.aardvarkI2cSpi.i2cSlaveAddress = m_userInterface->aardvarkI2cSlaveAddress->text().toUInt();
    m_currentSettings.aardvarkI2cSpi.i2cPullupsOn = (m_userInterface->aardvarkI2cPullUp->currentText() == "On") ? true : false;
    m_currentSettings.aardvarkI2cSpi.spiPolarity = (AardvarkSpiPolarity)m_userInterface->aardvarkSpiPolarity->currentIndex();
    m_currentSettings.aardvarkI2cSpi.spiSSPolarity = (AardvarkSpiSSPolarity)m_userInterface->aardvarkSpiSSPolarity->currentIndex();
    m_currentSettings.aardvarkI2cSpi.spiBitorder = (AardvarkSpiBitorder)m_userInterface->aardvarkSpiBitorder->currentIndex();
    m_currentSettings.aardvarkI2cSpi.spiPhase = (AardvarkSpiPhase)m_userInterface->aardvarkSpiPhase->currentIndex();
    m_currentSettings.aardvarkI2cSpi.spiBaudrate= m_userInterface->aardvarkSpiBaudrate->text().toUInt();

    for(int i = 0; i < AARDVARK_I2C_SPI_GPIO_COUNT; i++)
    {
        m_currentSettings.aardvarkI2cSpi.pinConfigs[i].isInput = (m_aardvarkI2cGpioGuiElements[i].mode->currentText().contains("in")) ? true : false;
        m_currentSettings.aardvarkI2cSpi.pinConfigs[i].withPullups = (m_aardvarkI2cGpioGuiElements[i].mode->currentText() == "in pullup") ? true : false;
        m_currentSettings.aardvarkI2cSpi.pinConfigs[i].outValue = (m_aardvarkI2cGpioGuiElements[i].outValue->currentText() == "1") ? true : false;
    }

}


/**
 * The state of the 'append time stamp at logs' check box has been changed.
 * @param newState
 *      The new state.
 */
void SettingsDialog::appendTimestampAtLogsChangedSlot(int newState)
{
    stateFromCheckboxChangedSlot(newState);
    emit appendTimestampAtLogsChangedSignal();
}
