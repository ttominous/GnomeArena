#pragma once

#include "CoreMinimal.h"
#include "FileTools.generated.h"

UCLASS()
class COREPLAY_API UFileTools : public UObject {

	GENERATED_BODY()

public:

	static FString rootContentPath;
	static FString rootSavePath;

	static FString getRootContentPath();
	static FString getRootSavePath();

	static bool fileExist(FString path);
	static bool folderExists(FString path);
	static TArray< FString > getFileList(FString path, FString extension = "", bool includeFullPath = true, bool includeSubFolders = false, bool suppressErrors = false);
	static TArray< FString > getSubFolderList(FString path, bool includeFullPath = true, bool includeSubFolders = false);
	static void saveBMP(FString path, TArray<FColor> colors, int width, int height);
	static void saveUInt8Array(FString path, TArray< uint8 > data);
	static TArray< uint8 > loadUInt8Array(FString path, int size);

	static FString getFileContents(FString path);
	static FString getFileExtension(FString path);
	static FString getFileName(FString path, bool withExtension = true);
	static void saveFile(FString path, FString content);

	static void makeFolder(FString path);
	static void deleteFilesInFolder(FString path);
	static void copyFilesFromFolder(FString pathA, FString pathB);
};
