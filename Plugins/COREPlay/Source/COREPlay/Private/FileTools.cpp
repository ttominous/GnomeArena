#include "FileTools.h"
#include "StringTools.h"
#include "Debug.h"

#include "HAL/FileManagerGeneric.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Misc/App.h"


FString UFileTools::rootContentPath = "";
FString UFileTools::rootSavePath = "";


FString UFileTools::getRootContentPath() {
	if (rootContentPath.Equals("")) {
		rootContentPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	}
	return rootContentPath;
}


FString UFileTools::getRootSavePath() {
	if (rootSavePath.Equals("")) {
		rootSavePath = FPlatformProcess::UserSettingsDir();
		rootSavePath += FApp::GetProjectName();
		makeFolder(rootSavePath);
	}
	return rootSavePath;
}

bool UFileTools::fileExist(FString path) {
	return FPaths::FileExists(path);
}

bool UFileTools::folderExists(FString path) {
	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	return platformFile.DirectoryExists(*path);
}


TArray< FString > UFileTools::getFileList(FString path, FString extension, bool includeFullPath, bool includeSubFolders, bool suppressErrors) {

	if (path.Contains("/") && !path.EndsWith("/")) {
		path += "/";
	}
	if (path.Contains("\\") && !path.EndsWith("\\")) {
		path += "\\";
	}
	path = path.Replace(TEXT("//"), TEXT("/"));

	TArray<FString> results;
	if (FPaths::DirectoryExists(path)) {

		TArray<FString> files;
		IFileManager& fileManager = IFileManager::Get();
		fileManager.FindFiles(files, *path, *extension);

		for (FString file : files) {
			if (includeFullPath) {
				results.Add(path + file);
			}
			else {
				results.Add(file);
			}
		}

		if (includeSubFolders) {
			TArray< FString > subFolders = getSubFolderList(path, true, true);
			for (FString subFolder : subFolders) {
				TArray< FString > subFiles = getFileList(subFolder, extension, includeFullPath, true);
				for (FString subFile : subFiles) {
					results.Add(subFile);
				}
			}
		}
	}
	else if (!suppressErrors) {
		UDebug::error(path + " doesn't exist!");
	}
	return results;
}


TArray< FString > UFileTools::getSubFolderList(FString path, bool includeFullPath, bool includeSubFolders) {

	if (path.Contains("/") && !path.EndsWith("/")) {
		path += "/";
	}
	if (path.Contains("\\") && !path.EndsWith("\\")) {
		path += "\\";
	}
	path = path.Replace(TEXT("//"), TEXT("/"));

	TArray<FString> results;
	if (FPaths::DirectoryExists(path)) {

		TArray<FString> subFolders;
		IFileManager& fileManager = IFileManager::Get();

		FString sweepPath = path / TEXT("*");
		fileManager.FindFiles(subFolders, *sweepPath, false, true);

		for (FString subFolder : subFolders) {
			if (includeFullPath) {
				results.Add(path + subFolder);
			}
			else {
				results.Add(subFolder);
			}

			if (includeSubFolders) {
				TArray< FString > childSubFolders = getSubFolderList(path + subFolder, true);
				for (FString childSubFolder : childSubFolders) {
					results.Add(childSubFolder);
				}
			}
		}
	}
	else {
		UDebug::error(path + " doesn't exist!");
	}
	return results;
}

void UFileTools::saveBMP(FString path, TArray<FColor> colors, int width, int height) {
	if (!FFileHelper::CreateBitmap(*path, width, height, colors.GetData())) {
		UDebug::error("Failed to save " + path);
	}
}

void UFileTools::saveUInt8Array(FString path, TArray< uint8 > data) {
	if (!FFileHelper::SaveArrayToFile(data, *path)) {
		UDebug::error("Could not save " + path);
	}
}

TArray< uint8 > UFileTools::loadUInt8Array(FString path, int size) {
	TArray<uint8> data;
	data.Init(0, size);
	if (!FFileHelper::LoadFileToArray(data, *path)) {
		UDebug::error("Could not load " + path);
	}
	return data;
}

FString UFileTools::getFileContents(FString path) {
	path = path.Replace(TEXT("//"), TEXT("/"));
	FString contents;
	FFileHelper::LoadFileToString(contents, *path);
	return contents;
}

FString UFileTools::getFileExtension(FString path) {
	FString left, right;
	path.Split(TEXT("."), &left, &right);
	return "." + right;
}

FString UFileTools::getFileName(FString path, bool withExtension) {
	TArray<FString> levels;
	path.ParseIntoArray(levels, TEXT("/"), true);

	FString name = levels[levels.Num() - 1];

	return withExtension ? name : name.Replace(*getFileExtension(name), TEXT(""), ESearchCase::IgnoreCase);
}

void UFileTools::saveFile(FString path, FString content) {
	path = path.Replace(TEXT("//"), TEXT("/"));
	FFileHelper::SaveStringToFile(content, *path);
}

void UFileTools::makeFolder(FString path) {
	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!platformFile.DirectoryExists(*path)) {
		platformFile.CreateDirectory(*path);
	}
}

void UFileTools::deleteFilesInFolder(FString path) {
	TArray< FString > paths = UFileTools::getFileList(path, "", true, true, true);
	if (paths.Num() > 0) {
		IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
		for (FString childPath : paths) {
			platformFile.DeleteFile(*childPath);
		}
	}
}

void UFileTools::copyFilesFromFolder(FString pathA, FString pathB) {
	TArray< FString > paths = UFileTools::getFileList(pathA, "", true, true, true);
	if (paths.Num() > 0) {
		makeFolder(pathB);
		IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
		for (FString childPath : paths) {
			FString destPath = childPath.Replace(*pathA, *pathB);
			platformFile.MoveFile(*destPath, *childPath);
		}
	}
}