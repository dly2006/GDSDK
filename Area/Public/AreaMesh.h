// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AreaMesh.generated.h"


class USplineComponent;
class UProceduralMeshComponent;
class UMaterialInterface;


struct MeshTriangle
{
	FVector point1=FVector();
	FVector point2=FVector();
	FVector point3=FVector();
};
UCLASS()
class LINE_API AAreaMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAreaMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	private:
	TArray<MeshTriangle> Triangles;
	TArray<FVector> Points;
	TArray<FVector> UpPoints;
	TArray<TArray<FVector>> SidePointsArrays;
	float  StandardSize=50.f;
	

	protected:
	//UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category="Default")
	//USplineComponent* pSpline=nullptr;
    UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite,Category="Default")
	UProceduralMeshComponent *pProcedural=nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Default")
	UMaterialInterface * pAreaMeshMaterial=nullptr;

    protected:
	UFUNCTION(BlueprintCallable)
	void CreateArea(const TArray<FVector>& points,int32& area,int32& length,int32& Perimeter);
	UFUNCTION(BlueprintCallable)
	void CreateAreaVolume(const TArray<FVector>& points,int32& area,int32& Volume,float Height=1.0f);
	void TriangulatePoints();
	void TriangulatePointsAsVolume( TArray<FVector>& splinePoints);
	void GetPolygonComponents(const TArray<FVector>& splinePoints,
							TArray<FVector>& reflexPoints, TArray<FVector>& convexPoints, TArray<FVector>& earPoints,
							TArray<int>& reflexIndices, TArray<int>& convexIndices, TArray<int>& earIndices);
	void GetPolygonComponentsAsVolume(const TArray<FVector>& splinePoints,
							TArray<FVector>& reflexPoints, TArray<FVector>& convexPoints, TArray<FVector>& earPoints,
							TArray<int>& reflexIndices, TArray<int>& convexIndices, TArray<int>& earIndices,float Height=1.0f);
	void TrianglesFromPoints(TArray<FVector>& splinePoints, const TArray<int>& inConvexIndices,
						const TArray<int>& inReflexIndices, const TArray<
							int>& inEarIndices);
	void TrianglesToIndices(const TArray<MeshTriangle>& triangles, TArray<FVector>& vertices,
							TArray<int>& indices) const;
	void CreateAreaMesh() const;

	double ComputePolygonArea() const;
	bool IsClockWise(const TArray<FVector>& points);
	UFUNCTION(BlueprintCallable)
	void GetLength(const TArray<FVector>& points,int32& length);
	UFUNCTION(BlueprintCallable)
	void Getperimeter(const TArray<FVector>& points,int32& Perimeter);
	void ClearData();
};
