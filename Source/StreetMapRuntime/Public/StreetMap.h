// Copyright 2017 Mike Fricker. All Rights Reserved.

#pragma once

#include "LandscapeProxy.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/DataTable.h"
#include "Misc/Crc.h"
#include "StreetMap.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStreetMap, All, All);

class FOSMNodeInfo;

/** Types of miscellaneous ways */
UENUM(BlueprintType)
enum EStreetMapMiscWayType
{
	/** unknown type */
	Unknown,

	/** The leisure tag is for places people go in their spare time (e.g. parks, pitches). */
	Leisure,

	/** Used to describe natural and physical land features (e.g. wood, beach, water). */
	Natural,

	/** Used to describe the primary use of land by humans (e.g. grass, meadow, forest). */
	LandUse,
};

UENUM(BlueprintType)
enum class EColorMode : uint8
{
	Default,
	Flow,
	Predictive0,
	Predictive15,
	Predictive30,
	Predictive45
};

USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapCollisionSettings
{
	GENERATED_USTRUCT_BODY()

public:


	/** Uses triangle mesh data for collision data. (Cannot be used for physics simulation). */
	UPROPERTY(EditAnywhere, Category = "StreetMap")
		uint32 bGenerateCollision : 1;

	/**
	*	If true, the physics triangle mesh will use double sided faces when doing scene queries.
	*	This is useful for planes and single sided meshes that need traces to work on both sides.
	*/
	UPROPERTY(EditAnywhere, Category = "StreetMap", meta = (editcondition = "bGenerateCollision"))
		uint32 bAllowDoubleSidedGeometry : 1;


	FStreetMapCollisionSettings() :
		bGenerateCollision(false),
		bAllowDoubleSidedGeometry(false)
	{

	}

};

/** Mesh generation settings */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapMeshBuildSettings
{
	GENERATED_USTRUCT_BODY()

public:

	/** Roads base vertical offset */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), DisplayName = "Street Vertical Offset")
		float StreetOffsetZ;
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), DisplayName = "Major Road Vertical Offset")
		float MajorRoadOffsetZ;
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), DisplayName = "Highway Vertical Offset")
		float HighwayOffsetZ;

	/** if true streets will be a single mesh instead of a list of quads. */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, DisplayName = "Smooth streets")
		uint32 bWantSmoothStreets : 1;

	/** if true streets of the same type that share nodes will be merged. */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, DisplayName = "Connect streets")
		uint32 bWantConnectStreets : 1;

	/** threshold for angle btween roads to merge */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), DisplayName = "Connect streets threshold")
		float fThresholdConnectStreets = 0.96;

	/** if true buildings mesh will be 3D instead of flat representation. */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, DisplayName = "Create 3D Buildings")
		uint32 bWant3DBuildings : 1;

	/** building level floor conversion factor in centimeters
		@todo: harmonize with OSMToCentimetersScaleFactor refactoring
	*/
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, DisplayName = "Building Level Floor Factor")
		float BuildingLevelFloorFactor = 300.0f;

	/** Default building height in centimeters - used if no height info is available */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, DisplayName = "Building Level Floor Factor")
		float BuildDefaultZ = 300.0f;

	/**
	* If true, buildings mesh will receive light information.
	* Lit buildings can't share vertices beyond quads (all quads have their own face normals), so this uses a lot more geometry.
	*/
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, DisplayName = "Lit buildings")
		uint32 bWantLitBuildings : 1;

	/** Streets thickness */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float StreetThickness;

	/** Street vertex color */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FLinearColor StreetColor;

	/** Major road thickness */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MajorRoadThickness;

	/** Major road vertex color */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FLinearColor MajorRoadColor;

	/** Highway thickness */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float HighwayThickness;

	/** Highway vertex color */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FLinearColor HighwayColor;

	/** Streets Thickness */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BuildingBorderThickness;

	/** Building border vertex color */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FLinearColor BuildingBorderLinearColor;

	/** Buildings border vertical offset */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BuildingBorderZ;

	/** Flow vertex colors */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FLinearColor LowFlowColor;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FLinearColor MedFlowColor;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FLinearColor HighFlowColor;

	UPROPERTY(Category = Status, EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EColorMode> ColorMode;

	FStreetMapMeshBuildSettings() :
		StreetOffsetZ(100.0f),
		MajorRoadOffsetZ(200.0f),
		HighwayOffsetZ(300.0f),
		bWantSmoothStreets(true),
		bWantConnectStreets(true),
		fThresholdConnectStreets(0.96),
		bWant3DBuildings(true),
		bWantLitBuildings(true),
		StreetThickness(800.0f),
		StreetColor(0.05f, 0.75f, 0.05f),
		MajorRoadThickness(1000.0f),
		MajorRoadColor(0.15f, 0.85f, 0.15f),
		HighwayThickness(1400.0f),
		HighwayColor(FLinearColor(0.25f, 0.95f, 0.25f)),
		BuildingBorderThickness(20.0f),
		BuildingBorderLinearColor(0.85f, 0.85f, 0.85f),
		BuildingBorderZ(10.0f),
		LowFlowColor(FLinearColor(1.0f, 0.0f, 0.0f)),
		MedFlowColor(FLinearColor(1.0f, 1.05f, 0.0f)),
		HighFlowColor(FLinearColor(0.2f, 0.8f, 0.0f)),
		ColorMode(EColorMode::Default)
	{

	}
};

/** Identifies a specific type of way */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FWayMatch
{
	GENERATED_USTRUCT_BODY()

public:

	/** The OSM type this way is marked as */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TEnumAsByte<EStreetMapMiscWayType> Type;

	// Minimal size of the Landscape in each direction around the center of the OpenStreetMap in meters.
	UPROPERTY(Category = "Landscape", EditAnywhere)
		FString Category;

	FWayMatch()
		: Type(EStreetMapMiscWayType::Unknown)
		, Category(TEXT(""))
	{
	}

	FWayMatch(EStreetMapMiscWayType Type, const FString& Category)
		: Type(Type)
		, Category(Category)
	{
	}
};


/** Maps multiple types of ways to a specific Landscae layer */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FLayerWayMapping
{
	GENERATED_USTRUCT_BODY()

public:

	// The Layer's name this mapping is used for
	UPROPERTY(Category = "Landscape", VisibleAnywhere)
		FName LayerName;

	// Types of ways that make this layer up
	UPROPERTY(Category = "Landscape", EditAnywhere)
		TArray<FWayMatch> Matches;

	FLayerWayMapping()
	{
	}
};


/** Landscape generation settings */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapLandscapeBuildSettings
{
	GENERATED_USTRUCT_BODY()

public:
	// Horizontal distance between elevation data points in meters. Keep in mind that elevation data is usually available in 10-30 meter resolution. Anything in between will be interpolated.
	UPROPERTY(Category = "Landscape", EditAnywhere, meta = (UIMin = 1, ClampMin = 0.25f, ClampMax = 100.0f))
		float QuadSize;

	// Minimal size of the Landscape in each direction around the center of the OpenStreetMap in meters.
	UPROPERTY(Category = "Landscape", EditAnywhere, meta = (UIMin = 1, ClampMin = 256, ClampMax = 16384))
		int32 Radius;

	// Width of the blend area between layers in meters.
	UPROPERTY(Category = "Landscape", EditAnywhere, meta = (UIMin = 1, ClampMin = 0.0f, ClampMax = 200.0f))
		float BlendGauge;

	// Material initially applied to the landscape. Setting a material here exposes properties for setting up layer info based on the landscape blend nodes in the material.
	UPROPERTY(Category = "Landscape", EditAnywhere, meta = (DisplayName = "Material", ShowForTools = "Landscape"))
		UMaterialInterface* Material;

	// The landscape layers that will be created. Only layer names referenced in the material assigned above are shown here. Modify the material to add more layers.
	UPROPERTY(Category = "Landscape", EditAnywhere, NonTransactional, EditFixedSize, meta = (DisplayName = "Layers", ShowForTools = "Landscape"))
		TArray<FLandscapeImportLayerInfo> Layers;

	// WayTypes corresponding to each layer. Only layer names referenced in the material assigned above are shown here. Modify the material to add more layers.
	UPROPERTY(Category = "Landscape", EditAnywhere, NonTransactional, EditFixedSize, meta = (DisplayName = "Layer Ways", ShowForTools = "Landscape"))
		TArray<FLayerWayMapping> LayerWayMapping;

	FStreetMapLandscapeBuildSettings()
		: QuadSize(4.0f)
		, Radius(8192)
		, BlendGauge(8.0f)
		, Material(nullptr)
	{
	}
};

/** Railway generation settings */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapRailwayBuildSettings
{
	GENERATED_USTRUCT_BODY()

public:

	// Landscape where to put the railways onto
	UPROPERTY(Category = "Railway", EditAnywhere)
		ALandscapeProxy* Landscape;

	// Track segment used to build the railroad line via Landscape Spline Meshes.
	UPROPERTY(Category = "Railway", EditAnywhere)
		UStaticMesh* RailwayLineMesh;

	// Scales mesh to width of landscape spline
	UPROPERTY(Category = "Road", EditAnywhere)
		bool ScaleToWidth = false;

	// Offset of the LandscapeSpline above the ground
	UPROPERTY(Category = "Railway", EditAnywhere)
		float ZOffset;

	// Falloff to the side of the LandscapeSpline
	UPROPERTY(Category = "Road", EditAnywhere)
		float SideFalloff;

	// Falloff at the end of the LandscapeSpline
	UPROPERTY(Category = "Road", EditAnywhere)
		float EndFalloff;

	/** Chooses the forward axis for the spline mesh orientation */
	UPROPERTY(Category = "Railway", EditAnywhere)
		TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	/** Chooses the up axis for the spline mesh orientation */
	UPROPERTY(Category = "Railway", EditAnywhere)
		TEnumAsByte<ESplineMeshAxis::Type> UpAxis;

	/** Width of the generated splines */
	UPROPERTY(Category = "Railway", EditAnywhere)
		float Width;

	FStreetMapRailwayBuildSettings()
		: Landscape(nullptr)
		, RailwayLineMesh(nullptr)
		, ZOffset(0.0f)
		, SideFalloff(1.5f)
		, EndFalloff(3.0f)
		, ForwardAxis(ESplineMeshAxis::X)
		, UpAxis(ESplineMeshAxis::Z)
		, Width(200.0f)
	{
	}
};


/** Roads as LandscapeSpline generation settings */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapRoadBuildSettings
{
	GENERATED_USTRUCT_BODY()

public:

	// Landscape where to put the roads onto
	UPROPERTY(Category = "Road", EditAnywhere)
		ALandscapeProxy* Landscape;

	// Track segment used to build the roads via Landscape Spline Meshes.
	UPROPERTY(Category = "Road", EditAnywhere)
		UStaticMesh* RoadMesh;

	// Scales mesh to width of landscape spline
	UPROPERTY(Category = "Road", EditAnywhere)
		bool ScaleToWidth = false;

	// Offset of the LandscapeSpline above the ground
	UPROPERTY(Category = "Road", EditAnywhere)
		float ZOffset;

	// Falloff to the side of the LandscapeSpline
	UPROPERTY(Category = "Road", EditAnywhere)
		float SideFalloff;

	// Falloff at the end of the LandscapeSpline
	UPROPERTY(Category = "Road", EditAnywhere)
		float EndFalloff;

	/** Chooses the forward axis for the spline mesh orientation */
	UPROPERTY(Category = "Road", EditAnywhere)
		TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	/** Chooses the up axis for the spline mesh orientation */
	UPROPERTY(Category = "Road", EditAnywhere)
		TEnumAsByte<ESplineMeshAxis::Type> UpAxis;

	FStreetMapRoadBuildSettings()
		: Landscape(nullptr)
		, RoadMesh(nullptr)
		, ZOffset(0.0f)
		, SideFalloff(1.5f)
		, EndFalloff(3.0f)
		, ForwardAxis(ESplineMeshAxis::X)
		, UpAxis(ESplineMeshAxis::Z)
	{
	}
};


/** Types of Splines to generate */
UENUM(BlueprintType)
enum EStreetMapSplineBuildType
{
	/** Cinematic Rig Rail */
	CinematicRigRail,
};


/** Generic Spline generation settings */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapSplineBuildSettings
{
	GENERATED_USTRUCT_BODY()

public:

	// Start Actor reference where to look for landscape spline references nearby to start the generated spline
	UPROPERTY(Category = "Spline", EditAnywhere)
		AActor* Start;

	// End Actor reference where to look for landscape spline references nearby to end the generated spline
	UPROPERTY(Category = "Spline", EditAnywhere)
		AActor* End;

	// Vertical offset of the spline above the landscape splines
	UPROPERTY(Category = "Spline", EditAnywhere)
		float ZOffset;

	/** What type of Spline Actor should be generated */
	UPROPERTY(Category = "Spline", EditAnywhere)
		TEnumAsByte<EStreetMapSplineBuildType> Type;

	FStreetMapSplineBuildSettings()
		: Start(nullptr)
		, End(nullptr)
		, ZOffset(0.0f)
		, Type(EStreetMapSplineBuildType::CinematicRigRail)
	{
	}
};

/** Types of vertices */
UENUM(BlueprintType)
enum EVertexType
{
	/** Small road or residential street */
	VStreet,

	/** Major road or minor state highway */
	VMajorRoad,

	/** Highway */
	VHighway,

	/** Building */
	VBuilding
};

/** Types of roads */
UENUM(BlueprintType)
enum EStreetMapRoadType
{
	/** Small road or residential street */
	Street,

	/** Major road or minor state highway */
	MajorRoad,

	/** Highway */
	Highway,

	/** Bridge */
	Bridge,

	/** Other (path, bus route, etc) */
	Other
};

/** T3 Link */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapLink : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		int64 LinkId;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FString LinkDir;

	FStreetMapLink(int64 linkId = 0, FString linkDir = "T")
	{
		LinkId = linkId;
		LinkDir = linkDir;
	}
};


/** T3 Trace */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapTrace
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FGuid GUID;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		TArray<FStreetMapLink> Links;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FLinearColor Color;
};

inline uint32 GetTypeHash(const FStreetMapLink& Value)
{
	uint32 LinkIdHash = GetTypeHash(Value.LinkId);
	uint32 LinkDirHash = GetTypeHash(Value.LinkDir);
	return LinkIdHash ^ LinkDirHash;
}

inline bool operator==(const FStreetMapLink& A, const FStreetMapLink& B)
{
	return (0 == A.LinkDir.Compare(B.LinkDir, ESearchCase::CaseSensitive)) &&
		(A.LinkId == B.LinkId);
}

inline bool operator!=(const FStreetMapLink& A, const FStreetMapLink& B)
{
	return !(A == B);
}


/** A road */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapRoad
{
	GENERATED_USTRUCT_BODY()

	/** Name of the road */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FString RoadName;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FStreetMapLink Link;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FName TMC;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		int SpeedLimit;

	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		float Distance;

	/** Type of road */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EStreetMapRoadType> RoadType;

	/** Nodes along this road, one at each point in the RoadPoints list */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		TArray<int32> NodeIndices;

	/** List of all of the points on this road, one for each node in the NodeIndices list */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> RoadPoints;

	// @todo: Performance: Bounding information could be computed at load time if we want to avoid the memory cost of storing it

	/** 2D bounds (min) of this road's points */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FVector2D BoundsMin;

	/** 2D bounds (max) of this road's points */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		FVector2D BoundsMax;

	/** True if this node is a one way.  One way nodes are only traversable in the order the nodes are listed in the above array. */
	UPROPERTY(Category = StreetMap, EditAnywhere, BlueprintReadWrite)
		uint8 bIsOneWay : 1;

	/** the texture V component at the start and end of the road */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FVector2D textureVStart = FVector2D(-1.f, -1.f);

	bool lengthComputed = false;

	/** Returns this node's index */
	inline int32 GetRoadIndex(const class UStreetMap& StreetMap) const;

	/** Gets the node for the specified point, or the node that came before that if the specified point doesn't have a node */
	inline const struct FStreetMapNode& GetNodeAtPointIndexOrEarlier(const class UStreetMap& StreetMap, const int32 PointIndex, int32& OutNodeAtPointIndex) const;

	/** Gets the node for the specified point, or the node that comes next after that if the specified point doesn't have a node */
	inline const struct FStreetMapNode& GetNodeAtPointIndexOrLater(const class UStreetMap& StreetMap, const int32 PointIndex, int32& OutNodeAtPointIndex) const;

	/** Computes the total length of this road by following along all of it's points */
	float ComputeLengthOfRoad(const class UStreetMap& StreetMap) const;

	/** Computes the distance along the road between two points on the road.  Be careful!  The same node can appear on a road twice. */
	float ComputeDistanceBetweenNodesOnRoad(const class UStreetMap& StreetMap, const int32 NodePointIndexA, const int32 NodePointIndexB) const;

	/** Given a position along the road, finds the nodes that come earlier and later on that road */
	void FindEarlierAndLaterNodesForPositionAlongRoad(const class UStreetMap& StreetMap, const float PositionAlongRoad, const FStreetMapNode*& OutEarlierNode, float& OutEarlierNodePositionAlongRoad, const FStreetMapNode*& OutLaterNode, float& OutLaterNodePositionAlongRoad) const;

	/** Given a node that exists at a point index on this road, finds the nodes that are immediately earlier and later to it (adjacent.)  Will set a nullptr if there are no earlier or later nodes */
	void FindEarlierAndLaterNodes(const class UStreetMap& StreetMap, const int32 RoadPointIndex, const FStreetMapNode*& OutEarlierNode, float& OutEarlierNodePositionAlongRoad, const FStreetMapNode*& OutLaterNode, float& OutLaterNodePositionAlongRoad) const;

	// NOTE: You may notice there is no "FindPointIndexForNode()" method in this class.  This is because
	//       the same node may appear more than once on any single road, so it's never safe to ask for
	//       a single point index on a road.

	/** Given a node that exists on this road, computes the position along this road of that node */
	float FindPositionAlongRoadForNode(const class UStreetMap& StreetMap, const int32 PointIndexForNode) const;

	/** Computes the location of a point along this road, given a distance along this road from the road's beginning */
	FVector2D MakeLocationAlongRoad(const class UStreetMap& StreetMap, const float PositionAlongRoad) const;

	/** @return True if this is a one way road */
	inline bool IsOneWay() const
	{
		return bIsOneWay == 1 ? true : false;
	}

	/** compute UVs V component to fit with connecting roads */
	void ComputeUVspan(float startV, float Thickness);

	/** Like ComputeUVspan but in reverse */
	void ComputeUVspanFromBack(float endV, float Thickness);
};


/** Nodes have a list of road refs, one for each road that intersects this node.  Each road ref references a road and also the
	point along that road where this node exists. */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapRoadRef
{
	GENERATED_USTRUCT_BODY()

		/** Index of road in the list of all roads in this street map */
		UPROPERTY(Category = StreetMap, EditAnywhere)
		int32 RoadIndex;

	/** Index of the point along road where this node exists */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		int32 RoadPointIndex;

	bool operator==(const int32& rhs) const
	{
		return RoadIndex == rhs;
	}
};

/** Nodes have a list of railway refs, one for each railway that intersects this node.  Each railway ref references a railway and also the
point along that railway where this node exists. */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapRailwayRef
{
	GENERATED_USTRUCT_BODY()

		/** Index of railway in the list of all railway in this street map */
		UPROPERTY(Category = StreetMap, EditAnywhere)
		int32 RailwayIndex;

	/** Index of the point along railway where this node exists */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		int32 RailwayPointIndex;
};


/** OSM Tag kept for later use */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapTag
{
	GENERATED_USTRUCT_BODY()

		/** Key of the OSM Tag */
		UPROPERTY(Category = StreetMap, EditAnywhere)
		FName Key;

	/** Value of the OSM Tag */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FName Value;
};


/** Describes a node on a road or railway. Nodes usually connect at least two roads/railways together, but they might also exist at the end of a dead-end street/railroad.  They are sort of like an "intersection". */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapNode
{
	GENERATED_USTRUCT_BODY()

		/** All of the roads that intersect this node.  We have references to each of these roads, as well as the point along each
			road where this node exists */
		UPROPERTY(Category = StreetMap, EditAnywhere)
		TArray<FStreetMapRoadRef> RoadRefs;

	/** All of the Railways that intersect this node.  We have references to each of these railways, as well as the point along each
		railway where this node exists */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TArray<FStreetMapRailwayRef> RailwayRefs;

	/** All Tags of this Node. Usually empty */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TArray<FStreetMapTag> Tags;

	/** 2D location of this node */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FVector2D Location;

	/** Returns this node's index */
	inline int32 GetNodeIndex(const UStreetMap& StreetMap) const;

	///
	/// Utility functions which may be useful for pathfinding algorithms (not used internally.)
	///

	/** Pathfinding: Given a node that is known to connect to this node via some road, searches for the road and returns it */
	inline const FStreetMapRoad& GetShortestCostRoadToNode(UStreetMap& StreetMap, const FStreetMapNode& OtherNode, const bool bIsTravelingForward, int32& OutPointIndexOnRoad) const;

	/** Pathfinding: Returns true if this node is the end point on a road with no connections */
	inline bool IsDeadEnd(const UStreetMap& StreetMap) const;

	/** Pathfinding: Returns the number of connections between this node and other roads, taking into account the direction of travel */
	inline int32 GetConnectionCount(const class UStreetMap& StreetMap, const bool bIsTravelingForward) const;

	/** Pathfinding: Returns a connected node by index (between 0 and GetConnectionCount() - 1 ), taking into account the direction of travel.  Also returns the connecting road and wherabouts on the road the connection occurs */
	inline const FStreetMapNode* GetConnection(const class UStreetMap& StreetMap, const int32 ConnectionIndex, const bool bIsTravelingForward, const struct FStreetMapRoad** OutConnectingRoad = nullptr, int32* OutPointIndexOnRoad = nullptr, int32* OutConnectedNodePointIndexOnRoad = nullptr) const;

	/** Pathfinding: Estimates the 'cost' of the specified connected by index (between 0 and GetConnectionCount() - 1) */
	inline float GetConnectionCost(const class UStreetMap& StreetMap, const int32 ConnectionIndex, const bool bIsTravelingForward) const;
};


/** Types of railways */
UENUM(BlueprintType)
enum EStreetMapRailwayType
{
	/** Full sized passenger or freight trains in the standard gauge for the country or state. */
	Rail,

	/** A higher-standard tram system, normally in its own right-of-way. */
	LightRail,

	/** A city passenger rail service running mostly grade separated. */
	Subway,

	/** One or two carriage rail vehicles, usually sharing motor road. */
	Tram,

	/** Other (monorail, abandoned, construction, disused, funicular, etc.) */
	OtherRailway,
};


/** A railway */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapRailway
{
	GENERATED_USTRUCT_BODY()

		/** Name of the railway */
		UPROPERTY(Category = StreetMap, EditAnywhere)
		FString Name;

	/** Type of railway */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TEnumAsByte<EStreetMapRailwayType> Type;

	/** Nodes along this railway, one at each point in the Points list */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TArray<int32> NodeIndices;

	/** List of all of the points on this railway */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TArray<FVector2D> Points;

	// @todo: Performance: Bounding information could be computed at load time if we want to avoid the memory cost of storing it

	/** 2D bounds (min) of this railway's points */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FVector2D BoundsMin;

	/** 2D bounds (max) of this railway's points */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FVector2D BoundsMax;
};



/** A building */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapBuilding
{
	GENERATED_USTRUCT_BODY()

		/** Name of the building */
		UPROPERTY(Category = StreetMap, EditAnywhere)
		FString BuildingName;

	/** Polygon points that define the perimeter of the building */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TArray<FVector2D> BuildingPoints;

	/** Height of the building in meters (if known, otherwise zero) */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		float Height;

	/** Levels of the building (if known, otherwise zero) */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		int BuildingLevels;

	// @todo: Performance: Bounding information could be computed at load time if we want to avoid the memory cost of storing it

	/** 2D bounds (min) of this building's points */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FVector2D BoundsMin;

	/** 2D bounds (max) of this building's points */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FVector2D BoundsMax;
};


/** A miscellaneous way */
USTRUCT(BlueprintType)
struct STREETMAPRUNTIME_API FStreetMapMiscWay
{
	GENERATED_USTRUCT_BODY()

		/** Name of the way */
		UPROPERTY(Category = StreetMap, EditAnywhere)
		FString Name;

	/** Category of the way */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FString Category;

	/** points that define the the way (line or polygon) */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TArray<FVector2D> Points;

	// @todo: Performance: Bounding information could be computed at load time if we want to avoid the memory cost of storing it

	/** 2D bounds (min) of this way's points */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FVector2D BoundsMin;

	/** 2D bounds (max) of this way's points */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		FVector2D BoundsMax;

	/** The OSM type this way is marked as */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		TEnumAsByte<EStreetMapMiscWayType> Type;

	/** Indicates whether this a closed polygon or just a line strip */
	UPROPERTY(Category = StreetMap, EditAnywhere)
		bool bIsClosed;
};

/** A loaded street map */
UCLASS()
class STREETMAPRUNTIME_API UStreetMap : public UObject
{
	GENERATED_BODY()

public:

	/** Default constructor for UStreetMap */
	UStreetMap();

	// UObject overrides
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;

	/** Gets the roads in this street map (read only) */
	const TArray<FStreetMapRoad>& GetRoads() const
	{
		return Roads;
	}

	/** Gets the roads in this street map */
	TArray<FStreetMapRoad>& GetRoads()
	{
		return Roads;
	}

	/** Gets the nodes on the map (read only.)  Nodes describe intersections between roads */
	UFUNCTION(BlueprintCallable, Category = "StreetMap")
		const TArray<FStreetMapNode>& GetNodes() const
	{
		return Nodes;
	}

	/** Gets the nodes on the map.  Nodes describe intersections between roads */
	TArray<FStreetMapNode>& GetNodes()
	{
		return Nodes;
	}

	/** Gets all of the buildings (read only) */
	const TArray<FStreetMapBuilding>& GetBuildings() const
	{
		return Buildings;
	}

	/** Gets all of the buildings */
	TArray<FStreetMapBuilding>& GetBuildings()
	{
		return Buildings;
	}

	/** Gets all of the railways (read only) */
	const TArray<FStreetMapRailway>& GetRailways() const
	{
		return Railways;
	}

	/** Gets all of the railways */
	TArray<FStreetMapRailway>& GetRailways()
	{
		return Railways;
	}

	/** Gets all of the miscellaneous ways (read only) */
	const TArray<FStreetMapMiscWay>& GetMiscWays() const
	{
		return MiscWays;
	}

	/** Gets all of the miscellaneous ways */
	TArray<FStreetMapMiscWay>& GetMiscWays()
	{
		return MiscWays;
	}


	/** Gets the bounding box of the map */
	FVector2D GetBoundsMin() const
	{
		return BoundsMin;
	}
	FVector2D GetBoundsMax() const
	{
		return BoundsMax;
	}

	double GetOriginLongitude() const
	{
		return OriginLongitude;
	}

	double GetOriginLatitude() const
	{
		return OriginLatitude;
	}

	UFUNCTION(BlueprintCallable, Category = "StreetMap")
		FVector2D GetOrigin() const
	{
		const FVector2D Origin(OriginLongitude, OriginLatitude);
		return Origin;
	}

protected:

	/** List of roads */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		TArray<FStreetMapRoad> Roads;

	/** List of nodes on this map.  Nodes describe interesting points along roads, usually where roads intersect or at the end of a dead-end street */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		TArray<FStreetMapNode> Nodes;

	/** List of all buildings on the street map */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		TArray<FStreetMapBuilding> Buildings;

	/** List of railways */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		TArray<FStreetMapRailway> Railways;

	/** List of all miscellaneous ways on the street map */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		TArray<FStreetMapMiscWay> MiscWays;

	/** 2D bounds (min) of this map's roads and buildings */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		FVector2D BoundsMin;

	/** 2D bounds (max) of this map's roads and buildings */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		FVector2D BoundsMax;

	/** Longitude Origin of the SpatialReferenceSystem */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		double OriginLongitude;
	/** Latitude Origin of the SpatialReferenceSystem */
	UPROPERTY(Category = StreetMap, VisibleAnywhere)
		double OriginLatitude;

#if WITH_EDITORONLY_DATA
	/** Importing data and options used for this mesh */
	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
		class UAssetImportData* AssetImportData;

	friend class UStreetMapFactory;
	friend class UStreetMapReimportFactory;
	friend class FStreetMapAssetTypeActions;
#endif	// WITH_EDITORONLY_DATA

};


inline int32 FStreetMapRoad::GetRoadIndex(const UStreetMap& StreetMap) const
{
	// Pointer arithmetic based on array start
	const int32 RoadIndex = this - StreetMap.GetRoads().GetData();
	return RoadIndex;
}


inline const FStreetMapNode& FStreetMapRoad::GetNodeAtPointIndexOrEarlier(const UStreetMap& StreetMap, const int32 PointIndex, int32& OutNodeAtPointIndex) const
{
	const FStreetMapNode* CurrentOrEarlierPointNode = nullptr;
	for (int32 NodePointIndex = PointIndex; NodePointIndex >= 0; --NodePointIndex)
	{
		if (NodeIndices[NodePointIndex] != INDEX_NONE)
		{
			CurrentOrEarlierPointNode = &StreetMap.GetNodes()[NodeIndices[NodePointIndex]];
			OutNodeAtPointIndex = NodePointIndex;
			break;
		}
	}
	return *CurrentOrEarlierPointNode;
}


inline const FStreetMapNode& FStreetMapRoad::GetNodeAtPointIndexOrLater(const UStreetMap& StreetMap, const int32 PointIndex, int32& OutNodeAtPointIndex) const
{
	const FStreetMapNode* NextOrUpcomingNode = nullptr;
	for (int32 NodePointIndex = PointIndex; NodePointIndex < RoadPoints.Num(); ++NodePointIndex)
	{
		if (NodeIndices[NodePointIndex] != INDEX_NONE)
		{
			NextOrUpcomingNode = &StreetMap.GetNodes()[NodeIndices[NodePointIndex]];
			OutNodeAtPointIndex = NodePointIndex;
			break;
		}
	}

	return *NextOrUpcomingNode;
}


inline float FStreetMapRoad::ComputeLengthOfRoad(const class UStreetMap& StreetMap) const
{
	// @todo: Performance: We could cache the road's total length at load time to avoid having to compute it,
	//        or we could save it right into the asset file

	return ComputeDistanceBetweenNodesOnRoad(StreetMap, 0, this->NodeIndices.Num() - 1);
}


inline float FStreetMapRoad::ComputeDistanceBetweenNodesOnRoad(const class UStreetMap& StreetMap, const int32 NodePointIndexA, const int32 NodePointIndexB) const
{
	float TotalDistanceSoFar = 0.0f;

	// NOTE: It is very important that we use the actual road point indices here and not nodes directly, because the same node can appear
	// more than once on a single road!

	// @todo: Performance: We can cache distances between connected nodes rather than computing them every time.  This
	//        can be computed at load time or at import time (and stored in the asset).  Most of the other functions
	//        in this class that perform Size() computations could be changed to use cached distances also!

	const int32 SmallerPointIndex = FMath::Max(0, FMath::Min(NodePointIndexA, NodePointIndexB));
	const int32 LargerPointIndex = FMath::Min(RoadPoints.Num() - 1, FMath::Max(NodePointIndexA, NodePointIndexB));

	for (int32 PointIndex = SmallerPointIndex; PointIndex < LargerPointIndex; ++PointIndex)
	{
		const FVector2D PointLocation = RoadPoints[PointIndex];
		const FVector2D NextPointLocation = RoadPoints[PointIndex + 1];

		const float DistanceBetweenPoints = (NextPointLocation - PointLocation).Size();

		TotalDistanceSoFar += DistanceBetweenPoints;
	}

	// @todo: Malformed data can cause this assertion to trigger.  This could be a single road with at least two adjacent nodes
	//        at the exact same location.  We need to filter this out at load time probably.
	// check( TotalDistanceSoFar > 0.0f );

	return TotalDistanceSoFar;
}


inline void FStreetMapRoad::FindEarlierAndLaterNodesForPositionAlongRoad(const class UStreetMap& StreetMap, const float PositionAlongRoad, const FStreetMapNode*& OutEarlierNode, float& OutEarlierNodePositionAlongRoad, const FStreetMapNode*& OutLaterNode, float& OutLaterNodePositionAlongRoad) const
{
	float CurrentPointPositionAlongRoad = 0.0f;

	const FStreetMapNode* EarlierStreetMapNode = nullptr;
	const FStreetMapNode* LaterStreetMapNode = nullptr;

	const int32 NumPoints = RoadPoints.Num();
	for (int32 CurrentPointIndex = 0; CurrentPointIndex < NumPoints - 1; ++CurrentPointIndex)
	{
		if (this->NodeIndices[CurrentPointIndex] != INDEX_NONE)
		{
			EarlierStreetMapNode = &StreetMap.GetNodes()[this->NodeIndices[CurrentPointIndex]];
			OutEarlierNodePositionAlongRoad = CurrentPointPositionAlongRoad;
		}

		const int32 NextPointIndex = CurrentPointIndex + 1;
		const FVector2D CurrentPointLocation = RoadPoints[CurrentPointIndex];
		const FVector2D NextPointLocation = RoadPoints[NextPointIndex];

		const float DistanceBetweenPoints = (NextPointLocation - CurrentPointLocation).Size();
		const float NextPointPositionAlongRoad = CurrentPointPositionAlongRoad + DistanceBetweenPoints;

		if (NextPointPositionAlongRoad >= PositionAlongRoad)
		{
			if (NodeIndices[NextPointIndex] != INDEX_NONE)
			{
				LaterStreetMapNode = &StreetMap.GetNodes()[this->NodeIndices[NextPointIndex]];
				OutLaterNodePositionAlongRoad = NextPointPositionAlongRoad;
				break;
			}
		}

		CurrentPointPositionAlongRoad = NextPointPositionAlongRoad;
	}

	check(EarlierStreetMapNode != nullptr && LaterStreetMapNode != nullptr);
	OutEarlierNode = EarlierStreetMapNode;
	OutLaterNode = LaterStreetMapNode;
}


inline void FStreetMapRoad::FindEarlierAndLaterNodes(const class UStreetMap& StreetMap, const int32 RoadPointIndex, const FStreetMapNode*& OutEarlierNode, float& OutEarlierNodePositionAlongRoad, const FStreetMapNode*& OutLaterNode, float& OutLaterNodePositionAlongRoad) const
{
	OutEarlierNode = nullptr;
	OutEarlierNodePositionAlongRoad = -1.0f;
	OutLaterNode = nullptr;
	OutLaterNodePositionAlongRoad = -1.0f;

	for (int32 EarlierPointIndex = RoadPointIndex - 1; EarlierPointIndex >= 0; --EarlierPointIndex)
	{
		if (this->NodeIndices[EarlierPointIndex] != INDEX_NONE)
		{
			OutEarlierNode = &StreetMap.GetNodes()[this->NodeIndices[EarlierPointIndex]];
			OutEarlierNodePositionAlongRoad = FindPositionAlongRoadForNode(StreetMap, EarlierPointIndex);
			break;
		}
	}

	for (int32 LaterPointIndex = RoadPointIndex + 1; LaterPointIndex < this->RoadPoints.Num(); ++LaterPointIndex)
	{
		if (this->NodeIndices[LaterPointIndex] != INDEX_NONE)
		{
			OutLaterNode = &StreetMap.GetNodes()[this->NodeIndices[LaterPointIndex]];
			OutLaterNodePositionAlongRoad = FindPositionAlongRoadForNode(StreetMap, LaterPointIndex);
			break;
		}
	}
}


inline float FStreetMapRoad::FindPositionAlongRoadForNode(const class UStreetMap& StreetMap, const int32 PointIndexForNode) const
{
	float CurrentPointPositionAlongRoad = 0.0f;

	bool bFoundLocation = false;
	const int32 NumPoints = RoadPoints.Num();
	for (int32 CurrentPointIndex = 0; CurrentPointIndex < PointIndexForNode; ++CurrentPointIndex)
	{
		const FVector2D CurrentPointLocation = RoadPoints[CurrentPointIndex];
		const FVector2D NextPointLocation = RoadPoints[CurrentPointIndex + 1];

		const float DistanceBetweenPoints = (NextPointLocation - CurrentPointLocation).Size();
		const float NextPointPositionAlongRoad = CurrentPointPositionAlongRoad + DistanceBetweenPoints;

		CurrentPointPositionAlongRoad = NextPointPositionAlongRoad;
	}

	return CurrentPointPositionAlongRoad;
}


inline FVector2D FStreetMapRoad::MakeLocationAlongRoad(const class UStreetMap& StreetMap, const float PositionAlongRoad) const
{
	FVector2D LocationAlongRoad = FVector2D::ZeroVector;
	float CurrentPointPositionAlongRoad = 0.0f;

	bool bFoundLocation = false;
	const int32 NumPoints = RoadPoints.Num();
	for (int32 CurrentPointIndex = 0; CurrentPointIndex < NumPoints - 1; ++CurrentPointIndex)
	{
		const FVector2D CurrentPointLocation = RoadPoints[CurrentPointIndex];
		const FVector2D NextPointLocation = RoadPoints[CurrentPointIndex + 1];

		const float DistanceBetweenPoints = (NextPointLocation - CurrentPointLocation).Size();
		const float NextPointPositionAlongRoad = CurrentPointPositionAlongRoad + DistanceBetweenPoints;

		if (NextPointPositionAlongRoad >= PositionAlongRoad)
		{
			const float LerpAlpha = (PositionAlongRoad - CurrentPointPositionAlongRoad) / DistanceBetweenPoints;
			LocationAlongRoad = FMath::Lerp(CurrentPointLocation, NextPointLocation, LerpAlpha);
			bFoundLocation = true;
			break;
		}

		CurrentPointPositionAlongRoad = NextPointPositionAlongRoad;
	}

	check(bFoundLocation == true);

	return LocationAlongRoad;
}


inline int32 FStreetMapNode::GetNodeIndex(const UStreetMap& StreetMap) const
{
	// Pointer arithmetic based on array start
	const int32 NodeIndex = this - StreetMap.GetNodes().GetData();
	return NodeIndex;
}


inline bool FStreetMapNode::IsDeadEnd(const UStreetMap& StreetMap) const
{
	if (RoadRefs.Num() == 1)
	{
		// @todo: If this road only connects to dead end roads that oppose the direction, we need to treat this road
		//        as a dead end.  This case should be extremely uncommon, though!

		const FStreetMapRoadRef& SoleRoadRef = RoadRefs[0];
		const FStreetMapRoad& SoleRoad = StreetMap.GetRoads()[SoleRoadRef.RoadIndex];
		if (SoleRoadRef.RoadPointIndex == 0 || SoleRoadRef.RoadPointIndex == (SoleRoad.NodeIndices.Num() - 1))
		{
			// The node is attached to only one road, and the node is at the very end of one of the ends of the road
			return true;
		}
	}

	return false;
}


inline const FStreetMapRoad& FStreetMapNode::GetShortestCostRoadToNode(UStreetMap& StreetMap, const FStreetMapNode& OtherNode, const bool bIsTravelingForward, int32& OutPointIndexOnRoad) const
{
	const FStreetMapRoad* ConnectingRoad = nullptr;

	float BestConnectionCost = TNumericLimits<float>::Max();
	int32 BestConnectionIndex = INDEX_NONE;
	int32 BestConnectionPointIndex = INDEX_NONE;

	const int32 ConnectionCount = GetConnectionCount(StreetMap, bIsTravelingForward);
	for (int32 ConnectionIndex = 0; ConnectionIndex < ConnectionCount; ++ConnectionIndex)
	{
		int32 MyPointIndexOnRoad;
		int32 ConnectedNodePointIndexOnRoad;

		const FStreetMapRoad* CurrentConnectingRoad;
		const FStreetMapNode* ConnectedNode = GetConnection(StreetMap, ConnectionIndex, bIsTravelingForward, /* Out */ &CurrentConnectingRoad, /* Out */ &MyPointIndexOnRoad, /* Out */ &ConnectedNodePointIndexOnRoad);
		if (ConnectedNode == &OtherNode)
		{
			if (BestConnectionIndex != INDEX_NONE)
			{
				// We evaluate cost lazily, only in the unusual case of the two nodes being connected by multiple roads
				BestConnectionCost = GetConnectionCost(StreetMap, BestConnectionIndex, bIsTravelingForward);
				if (GetConnectionCost(StreetMap, ConnectionIndex, bIsTravelingForward) < BestConnectionCost)
				{
					ConnectingRoad = CurrentConnectingRoad;
					BestConnectionIndex = ConnectionIndex;
					BestConnectionPointIndex = MyPointIndexOnRoad;
				}
			}
			else
			{
				ConnectingRoad = CurrentConnectingRoad;
				BestConnectionIndex = ConnectionIndex;
				BestConnectionPointIndex = MyPointIndexOnRoad;
			}
		}
	}

	check(ConnectingRoad != nullptr);
	OutPointIndexOnRoad = BestConnectionPointIndex;
	return *ConnectingRoad;
}

inline int32 FStreetMapNode::GetConnectionCount(const UStreetMap& StreetMap, const bool bIsTravelingForward) const
{
	// NOTE: We're iterating here in the exact same order as in the GetConnection() function below!  That's critically important!
	int32 TotalConnections = 0;
	for (const FStreetMapRoadRef& RoadRef : RoadRefs)
	{
		const FStreetMapRoad& Road = StreetMap.GetRoads()[RoadRef.RoadIndex];

		if (RoadRef.RoadPointIndex > 0 && (!bIsTravelingForward || !Road.IsOneWay()))
		{
			// We connect to a node earlier up this road
			++TotalConnections;
		}

		if (RoadRef.RoadPointIndex < (Road.NodeIndices.Num() - 1) && (bIsTravelingForward || !Road.IsOneWay()))
		{
			// We connect to a node further down this road
			++TotalConnections;
		}
	}

	return TotalConnections;
}


inline const FStreetMapNode* FStreetMapNode::GetConnection(const UStreetMap& StreetMap, const int32 ConnectionIndex, const bool bIsTravelingForward, const FStreetMapRoad** OutConnectingRoad, int32* OutPointIndexOnRoad, int32* OutConnectedNodePointIndexOnRoad) const
{
	if (OutConnectingRoad != nullptr)
	{
		*OutConnectingRoad = nullptr;
	}
	const FStreetMapNode* ConnectedNode = nullptr;

	// @todo: Performance: We can improve performance by caching additional connectivity information right on
	//        the node itself.  This function would be a hot spot for any sort of pathfinding computation

	// NOTE: We're iterating here in the exact same order as in the GetConnectionCount() function above!  That's critically important!
	int32 CurrentConnectionIndex = 0;
	for (const FStreetMapRoadRef& RoadRef : RoadRefs)
	{
		const FStreetMapRoad& Road = StreetMap.GetRoads()[RoadRef.RoadIndex];

		// @todo: Performance: We could avoid the "while" loops below by not storing INDEX_NONEs in the NodeIndices array,
		//        but instead mapping them to points by going through the node itself, then back to a road

		if (RoadRef.RoadPointIndex > 0 && (!bIsTravelingForward || !Road.IsOneWay()))
		{
			// We connect to an earlier node up this road
			if (CurrentConnectionIndex == ConnectionIndex)
			{
				int32 EarlierNodeRoadPointIndex = RoadRef.RoadPointIndex - 1;
				while (Road.NodeIndices[EarlierNodeRoadPointIndex] == INDEX_NONE)
				{
					--EarlierNodeRoadPointIndex;
				}
				const int32 EarlierNodeIndex = Road.NodeIndices[EarlierNodeRoadPointIndex];

				const FStreetMapNode& EarlierNode = StreetMap.GetNodes()[EarlierNodeIndex];
				ConnectedNode = &EarlierNode;
				if (OutConnectingRoad != nullptr)
				{
					*OutConnectingRoad = &Road;
				}
				if (OutPointIndexOnRoad != nullptr)
				{
					*OutPointIndexOnRoad = RoadRef.RoadPointIndex;
				}
				if (OutConnectedNodePointIndexOnRoad != nullptr)
				{
					*OutConnectedNodePointIndexOnRoad = EarlierNodeRoadPointIndex;
				}

				// Got it!
				break;
			}

			++CurrentConnectionIndex;
		}

		if (RoadRef.RoadPointIndex < (Road.NodeIndices.Num() - 1) && (bIsTravelingForward || !Road.IsOneWay()))
		{
			// We connect to node further down this road
			if (CurrentConnectionIndex == ConnectionIndex)
			{
				int32 LaterNodeRoadPointIndex = RoadRef.RoadPointIndex + 1;
				while (Road.NodeIndices[LaterNodeRoadPointIndex] == INDEX_NONE)
				{
					++LaterNodeRoadPointIndex;
				}
				const int32 LaterNodeIndex = Road.NodeIndices[LaterNodeRoadPointIndex];

				const FStreetMapNode& LaterNode = StreetMap.GetNodes()[LaterNodeIndex];
				ConnectedNode = &LaterNode;
				if (OutConnectingRoad != nullptr)
				{
					*OutConnectingRoad = &Road;
				}
				if (OutPointIndexOnRoad != nullptr)
				{
					*OutPointIndexOnRoad = RoadRef.RoadPointIndex;
				}
				if (OutConnectedNodePointIndexOnRoad != nullptr)
				{
					*OutConnectedNodePointIndexOnRoad = LaterNodeRoadPointIndex;
				}

				// Got it!
				break;
			}

			++CurrentConnectionIndex;
		}
	}

	check(ConnectedNode != nullptr);
	return ConnectedNode;
}


inline float FStreetMapNode::GetConnectionCost(const UStreetMap& StreetMap, const int32 ConnectionIndex, const bool bIsTravelingForward) const
{
	/////////////////////////////////////////////////////////
	// Tweakables for connection cost estimation
	//
	const float MaxSpeedLimit = 120.0f;	// 120 Km/hr
	const float HighwaySpeed = 110.0f;
	const float HighwayTrafficFactor = 0.0;
	const float MajorRoadSpeed = 70.0f;
	const float MajorRoadTrafficFactor = 0.2f;
	const float StreetSpeed = 40.0f;
	const float StreetTrafficFactor = 1.0f;
	/////////////////////////////////////////////////////////

	// @todo: Street map pathfinding is a grand art in itself, and estimating cost of connections is
	//        a very complicated problem.  We're only doing some basic estimates for now, but in the
	//        future we could consider taking into account the cost of different types of turns and
	//        intersections, lane counts, actual speed limits, etc.

	int32 MyPointIndexOnRoad;
	int32 ConnectedNodePointIndexOnRoad;

	const FStreetMapRoad* ConnectingRoad = nullptr;
	const FStreetMapNode& ConnectedNode = *GetConnection(StreetMap, ConnectionIndex, bIsTravelingForward, /* Out */ &ConnectingRoad, /* Out */ &MyPointIndexOnRoad, /* Out */ &ConnectedNodePointIndexOnRoad);

	const float DistanceBetweenNodes = ConnectingRoad->ComputeDistanceBetweenNodesOnRoad(StreetMap, MyPointIndexOnRoad, ConnectedNodePointIndexOnRoad);

	float TotalCost = DistanceBetweenNodes;

	// Apply some scaling to the cost of traveling between these nodes
	{
		float SpeedLimit = 0.0f;
		float TrafficFactor = 0.0f;
		switch (ConnectingRoad->RoadType)
		{
		case EStreetMapRoadType::Highway:
			SpeedLimit = HighwaySpeed;
			TrafficFactor = HighwayTrafficFactor;
			break;

		case EStreetMapRoadType::MajorRoad:
			SpeedLimit = MajorRoadSpeed;
			TrafficFactor = MajorRoadTrafficFactor;
			break;

		case EStreetMapRoadType::Street:
		case EStreetMapRoadType::Other:
			SpeedLimit = StreetSpeed;
			TrafficFactor = StreetTrafficFactor;
			break;

		default:
			check(0);
			break;
		}

		const float RoadSpeedCostScale = (1.0f - (SpeedLimit / MaxSpeedLimit));
		TotalCost *= 1.0f + RoadSpeedCostScale * 15.0f * (0.5f + TrafficFactor * 0.5f);
	}

	return TotalCost;
}


