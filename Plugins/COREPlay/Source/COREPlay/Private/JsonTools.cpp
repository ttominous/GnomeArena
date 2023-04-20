#include "JsonTools.h"
#include "StringTools.h"
#include "Debug.h"

TSharedPtr<FJsonObject> UJsonTools::parseJSON(FString jsonString) {
    TSharedPtr<FJsonValue> jsonValue;
    TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(jsonString);
    if (FJsonSerializer::Deserialize(jsonReader, jsonValue)) {
       return jsonValue->AsObject();
    }
    return nullptr;
}

TArray< TSharedPtr<FJsonValue > > UJsonTools::parseJSONArray(FString jsonString) {
    TSharedPtr<FJsonValue> jsonValue;
    TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(jsonString);
    if (FJsonSerializer::Deserialize(jsonReader, jsonValue)) {
        return jsonValue->AsArray();
    }
    return TArray< TSharedPtr<FJsonValue > >();
}

FString UJsonTools::toString(TSharedPtr<FJsonObject> jsonObject, bool escapeQuotes) {
    FString result;
    TSharedRef< TJsonWriter<> > jsonWriter = TJsonWriterFactory<>::Create(&result);
    FJsonSerializer::Serialize(jsonObject.ToSharedRef(), jsonWriter);

    if (escapeQuotes) {
        result = UStringTools::escapeQuotes(result);
    }

    return result;
}


TSharedPtr<FJsonObject> UJsonTools::makeObject() {
    return MakeShared<FJsonObject>();
}

void UJsonTools::setObject(TSharedPtr<FJsonObject> parent, FString key, TSharedPtr<FJsonObject> child) {
    parent->SetObjectField(key, child);
}

TSharedPtr<FJsonObject> UJsonTools::getObject(TSharedPtr<FJsonObject> parent, FString key) {
    return parent->GetObjectField(key);
}


void UJsonTools::setObjectArray(TSharedPtr<FJsonObject> parent, FString key, TArray< TSharedPtr<FJsonObject> > children) {
    TArray< TSharedPtr<FJsonValue> > values;
    for (TSharedPtr<FJsonObject> child : children) {
        values.Add(MakeShared<FJsonValueObject>(child));
    }
    parent->SetArrayField(key, values);
}

TArray < TSharedPtr<FJsonObject> > UJsonTools::getObjectArray(TSharedPtr<FJsonObject> parent, FString key) {
    TArray< TSharedPtr<FJsonObject> > children;
    TArray< TSharedPtr<FJsonValue> > values = parent->GetArrayField(key);
    for (TSharedPtr<FJsonValue> value : values) {
        children.Add(value->AsObject());
    }
    return children;
}


void UJsonTools::setString(TSharedPtr<FJsonObject> parent, FString key, FString value) {
    parent->SetStringField(key, value);
}

FString UJsonTools::getString(TSharedPtr<FJsonObject> parent, FString key) {
    return parent->GetStringField(key);
}

void UJsonTools::setColor(TSharedPtr<FJsonObject> parent, FString key, FColor value) {
    TSharedPtr<FJsonObject> valueObject = UJsonTools::makeObject();
    valueObject->SetNumberField("R", value.R);
    valueObject->SetNumberField("G", value.G);
    valueObject->SetNumberField("B", value.B);
    valueObject->SetNumberField("A", value.A);
    setObject(parent, key, valueObject);
}

FColor UJsonTools::getColor(TSharedPtr<FJsonObject> parent, FString key) {
    FColor result;
    TSharedPtr<FJsonObject> valueObject = getObject(parent, key);
    result.R = valueObject->GetNumberField("R");
    result.G = valueObject->GetNumberField("G");
    result.B = valueObject->GetNumberField("B");
    result.A = valueObject->GetNumberField("A");
    return result;
}


void UJsonTools::setNumeric(TSharedPtr<FJsonObject> parent, FString key, double value) {
    parent->SetNumberField(key, value);
}

double UJsonTools::getNumeric(TSharedPtr<FJsonObject> parent, FString key) {
    return parent->GetNumberField(key);
}