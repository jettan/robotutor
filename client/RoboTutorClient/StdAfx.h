// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <SDKDDKVer.h>

#include <memory>
#include <functional>
#include <type_traits>
#include <string>
#include <iostream>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/optional.hpp>
#include <boost/asio/io_service.hpp>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>

#include <QtWidgets>
#include <QtWidgets/QApplication>
#include <QString>
#include <QPlainTextEdit>
#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QLabel>
#include <QTimer>
#include <QDomDocument>
#include <QSyntaxHighlighter>

#include "protocol/messages.pb.h"
#include "connection.hpp"
#include "client.hpp"
#include "protobuf.hpp"

//#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
//	rename("RGB", "MSORGB") \
//	rename("DocumentProperties", "MSODocumentProperties")
// [-or-]
#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
	rename("RGB", "MSORGB") \
	rename("DocumentProperties", "MSODocumentProperties")

using namespace Office;

//#import "libid:0002E157-0000-0000-C000-000000000046"
// [-or-]
#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

using namespace VBIDE;

//#import "libid:91493440-5A91-11CF-8700-00AA0060263B" \
//	rename("RGB", "VisioRGB")
// [-or-]
#import "C:\\Program Files\\Microsoft Office\\Office12\\MSPPT.OLB" \
	rename("RGB", "VisioRGB")

#include "ui_RoboTutorClient.h"
#include "ScriptHighlighter.h"
#include "CodeEditor.h"
#include "PptController.h"
#include "AsioThread.h"
#include "RoboTutorClient.h"

