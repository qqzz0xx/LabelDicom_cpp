#include "AppMain.h"
#include "Header.h"

#include <QApplication>
#include <QMessageBox>

#include "api/license++.h"
#include "pc-identifiers.h"
#include "ini/SimpleIni.h"

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

int main(int argc, char *argv[])
{
	AppMain app(argc, argv);
	int code = 0;

	std::map<EVENT_TYPE, std::string> stringByEventType;
	stringByEventType[LICENSE_OK] = "OK ";
	stringByEventType[LICENSE_FILE_NOT_FOUND] = "license file not found ";
	stringByEventType[LICENSE_SERVER_NOT_FOUND] = "license server can't be contacted ";
	stringByEventType[ENVIRONMENT_VARIABLE_NOT_DEFINED] = "environment variable not defined ";
	stringByEventType[FILE_FORMAT_NOT_RECOGNIZED] = "license file has invalid format (not .ini file) ";
	stringByEventType[LICENSE_MALFORMED] = "some mandatory field are missing, or data can't be fully read. ";
	stringByEventType[PRODUCT_NOT_LICENSED] = "this product was not licensed ";
	stringByEventType[PRODUCT_EXPIRED] = "license expired ";
	stringByEventType[LICENSE_CORRUPTED] = "license signature didn't match with current license ";
	stringByEventType[IDENTIFIERS_MISMATCH] = "Calculated identifier and the one provided in license didn't match";
	stringByEventType[LICENSE_FILE_FOUND] = "license file not found ";
	stringByEventType[LICENSE_VERIFIED] = "license verified ";

	auto root = qApp->applicationDirPath();

	const std::string licLocation(root.toStdString() + "/RayShape-Anno.lic");

	LicenseInfo licenseInfo;
	LicenseLocation licenseLocation;
	licenseLocation.openFileNearModule = false;
	licenseLocation.licenseFileLocation = licLocation.c_str();
	licenseLocation.environmentVariableName = "";
	EVENT_TYPE result = acquire_license("RayShape-Anno", licenseLocation, &licenseInfo);
	PcSignature signature;
	FUNCTION_RETURN generate_ok = generate_user_pc_signature(signature, DEFAULT);

	if (result == LICENSE_OK && licenseInfo.linked_to_pc) {
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(licLocation.c_str());
		std::string IDinLicense = ini.GetValue("RayShape-Anno", "client_signature", "");
		if (IDinLicense == "") {
			std::cout << "No client signature in license file, generate license with -s <id>";
			result = IDENTIFIERS_MISMATCH;
		}
		else if (IDinLicense != signature) {
			result = IDENTIFIERS_MISMATCH;
		}
	}

	if (result != LICENSE_OK) {
		std::stringstream ss;
		ss << "license ERROR :" << std::endl;
		ss << "    " << stringByEventType[result].c_str() << std::endl;
		ss << "the pc id is :" << std::endl;
		ss << "    " << signature << std::endl;

		QMessageBox mb(QMessageBox::Information, "Tips",
			QString::fromStdString(ss.str()) , QMessageBox::Ok, nullptr);
		mb.setTextInteractionFlags(Qt::TextSelectableByMouse);
		code = mb.exec();

		return code;
	}
	else
	{
		std::cout << "license OK" << std::endl;
		
		app.Enter();
		app.Run();

		code = app.exec();

		app.Exit();

		return code;
	}
}