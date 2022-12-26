// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Oxi/OxiWeapon.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeOxiWeapon() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	ENGINE_API UClass* Z_Construct_UClass_UCameraComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_USkeletalMeshComponent_NoRegister();
	OXI_API UClass* Z_Construct_UClass_AItem();
	OXI_API UClass* Z_Construct_UClass_AItem_NoRegister();
	OXI_API UClass* Z_Construct_UClass_AOxiWeapon();
	OXI_API UClass* Z_Construct_UClass_AOxiWeapon_NoRegister();
	UPackage* Z_Construct_UPackage__Script_Oxi();
// End Cross Module References
	void AItem::StaticRegisterNativesAItem()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AItem);
	UClass* Z_Construct_UClass_AItem_NoRegister()
	{
		return AItem::StaticClass();
	}
	struct Z_Construct_UClass_AItem_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AItem_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_Oxi,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AItem_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "BlueprintType", "true" },
		{ "Comment", "/**\n *\n */" },
		{ "IncludePath", "OxiWeapon.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "OxiWeapon.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AItem_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AItem>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_AItem_Statics::ClassParams = {
		&AItem::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_AItem_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AItem_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AItem()
	{
		if (!Z_Registration_Info_UClass_AItem.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AItem.OuterSingleton, Z_Construct_UClass_AItem_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_AItem.OuterSingleton;
	}
	template<> OXI_API UClass* StaticClass<AItem>()
	{
		return AItem::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(AItem);
	AItem::~AItem() {}
	struct OxiWeapon_eventStartFireWeapon_Parms
	{
		const UCameraComponent* FirstPersonCameraComponent;
		bool bIsOutline;
		bool ReturnValue;

		/** Constructor, initializes return property only **/
		OxiWeapon_eventStartFireWeapon_Parms()
			: ReturnValue(false)
		{
		}
	};
	static FName NAME_AOxiWeapon_StartFireWeapon = FName(TEXT("StartFireWeapon"));
	bool AOxiWeapon::StartFireWeapon(const UCameraComponent* FirstPersonCameraComponent, bool bIsOutline)
	{
		OxiWeapon_eventStartFireWeapon_Parms Parms;
		Parms.FirstPersonCameraComponent=FirstPersonCameraComponent;
		Parms.bIsOutline=bIsOutline ? true : false;
		ProcessEvent(FindFunctionChecked(NAME_AOxiWeapon_StartFireWeapon),&Parms);
		return !!Parms.ReturnValue;
	}
	void AOxiWeapon::StaticRegisterNativesAOxiWeapon()
	{
	}
	struct Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_FirstPersonCameraComponent_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_FirstPersonCameraComponent;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bIsOutline_MetaData[];
#endif
		static void NewProp_bIsOutline_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsOutline;
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_FirstPersonCameraComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "NativeConst", "" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_FirstPersonCameraComponent = { "FirstPersonCameraComponent", nullptr, (EPropertyFlags)0x0010000000080082, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(OxiWeapon_eventStartFireWeapon_Parms, FirstPersonCameraComponent), Z_Construct_UClass_UCameraComponent_NoRegister, METADATA_PARAMS(Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_FirstPersonCameraComponent_MetaData, UE_ARRAY_COUNT(Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_FirstPersonCameraComponent_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_bIsOutline_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	void Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_bIsOutline_SetBit(void* Obj)
	{
		((OxiWeapon_eventStartFireWeapon_Parms*)Obj)->bIsOutline = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_bIsOutline = { "bIsOutline", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(OxiWeapon_eventStartFireWeapon_Parms), &Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_bIsOutline_SetBit, METADATA_PARAMS(Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_bIsOutline_MetaData, UE_ARRAY_COUNT(Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_bIsOutline_MetaData)) };
	void Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((OxiWeapon_eventStartFireWeapon_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(OxiWeapon_eventStartFireWeapon_Parms), &Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_FirstPersonCameraComponent,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_bIsOutline,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::Function_MetaDataParams[] = {
		{ "Category", "Animations" },
		{ "ModuleRelativePath", "OxiWeapon.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AOxiWeapon, nullptr, "StartFireWeapon", nullptr, nullptr, sizeof(OxiWeapon_eventStartFireWeapon_Parms), Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x0C020800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AOxiWeapon_StartFireWeapon()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AOxiWeapon_StartFireWeapon_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AOxiWeapon);
	UClass* Z_Construct_UClass_AOxiWeapon_NoRegister()
	{
		return AOxiWeapon::StaticClass();
	}
	struct Z_Construct_UClass_AOxiWeapon_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Mesh1P_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_Mesh1P;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ClipSize_MetaData[];
#endif
		static const UECodeGen_Private::FUnsizedIntPropertyParams NewProp_ClipSize;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CurrentAmmoCount_MetaData[];
#endif
		static const UECodeGen_Private::FUnsizedIntPropertyParams NewProp_CurrentAmmoCount;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AOxiWeapon_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AItem,
		(UObject* (*)())Z_Construct_UPackage__Script_Oxi,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_AOxiWeapon_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_AOxiWeapon_StartFireWeapon, "StartFireWeapon" }, // 3537899382
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AOxiWeapon_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "BlueprintType", "true" },
		{ "Comment", "/**\n *\n */" },
		{ "IncludePath", "OxiWeapon.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "OxiWeapon.h" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AOxiWeapon_Statics::NewProp_Mesh1P_MetaData[] = {
		{ "Category", "Weapon" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "OxiWeapon.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AOxiWeapon_Statics::NewProp_Mesh1P = { "Mesh1P", nullptr, (EPropertyFlags)0x002008000008000d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AOxiWeapon, Mesh1P), Z_Construct_UClass_USkeletalMeshComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_AOxiWeapon_Statics::NewProp_Mesh1P_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AOxiWeapon_Statics::NewProp_Mesh1P_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AOxiWeapon_Statics::NewProp_ClipSize_MetaData[] = {
		{ "Category", "Weapon" },
		{ "ModuleRelativePath", "OxiWeapon.h" },
	};
#endif
	const UECodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_AOxiWeapon_Statics::NewProp_ClipSize = { "ClipSize", nullptr, (EPropertyFlags)0x0020080000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AOxiWeapon, ClipSize), METADATA_PARAMS(Z_Construct_UClass_AOxiWeapon_Statics::NewProp_ClipSize_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AOxiWeapon_Statics::NewProp_ClipSize_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AOxiWeapon_Statics::NewProp_CurrentAmmoCount_MetaData[] = {
		{ "Category", "Weapon" },
		{ "ModuleRelativePath", "OxiWeapon.h" },
	};
#endif
	const UECodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_AOxiWeapon_Statics::NewProp_CurrentAmmoCount = { "CurrentAmmoCount", nullptr, (EPropertyFlags)0x0020080000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AOxiWeapon, CurrentAmmoCount), METADATA_PARAMS(Z_Construct_UClass_AOxiWeapon_Statics::NewProp_CurrentAmmoCount_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AOxiWeapon_Statics::NewProp_CurrentAmmoCount_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AOxiWeapon_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AOxiWeapon_Statics::NewProp_Mesh1P,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AOxiWeapon_Statics::NewProp_ClipSize,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AOxiWeapon_Statics::NewProp_CurrentAmmoCount,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AOxiWeapon_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AOxiWeapon>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_AOxiWeapon_Statics::ClassParams = {
		&AOxiWeapon::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_AOxiWeapon_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_AOxiWeapon_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_AOxiWeapon_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AOxiWeapon_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AOxiWeapon()
	{
		if (!Z_Registration_Info_UClass_AOxiWeapon.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AOxiWeapon.OuterSingleton, Z_Construct_UClass_AOxiWeapon_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_AOxiWeapon.OuterSingleton;
	}
	template<> OXI_API UClass* StaticClass<AOxiWeapon>()
	{
		return AOxiWeapon::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(AOxiWeapon);
	AOxiWeapon::~AOxiWeapon() {}
	struct Z_CompiledInDeferFile_FID_Oxi_Source_Oxi_OxiWeapon_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Oxi_Source_Oxi_OxiWeapon_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_AItem, AItem::StaticClass, TEXT("AItem"), &Z_Registration_Info_UClass_AItem, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AItem), 3454834950U) },
		{ Z_Construct_UClass_AOxiWeapon, AOxiWeapon::StaticClass, TEXT("AOxiWeapon"), &Z_Registration_Info_UClass_AOxiWeapon, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AOxiWeapon), 4184531677U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Oxi_Source_Oxi_OxiWeapon_h_3734556824(TEXT("/Script/Oxi"),
		Z_CompiledInDeferFile_FID_Oxi_Source_Oxi_OxiWeapon_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Oxi_Source_Oxi_OxiWeapon_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
