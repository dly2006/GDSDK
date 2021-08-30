// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaMesh.h"
#include <string>

#include "ProceduralMeshComponent.h"

// Sets default values
AAreaMesh::AAreaMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	pProcedural=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("AreaMesh"));
	pProcedural->SetupAttachment(RootComponent);
	pProcedural->bUseAsyncCooking=true;

}

void AAreaMesh::BeginPlay()
{
	Super::BeginPlay();	
}


void AAreaMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


inline TArray<FVector> reverseArray(TArray<FVector> a) 
{
	FVector temp ;
	int n = a.Num();
	for (int i = 0; i < n/2; ++i)
		{
		temp = a[n-i-1];
		a[n-i-1] = a[i];
		a[i] = temp;
	    }
	return a;
}
inline int CircularIndex(const int index, const int length)
{
    int temp = index % length;
    int temp1 = temp + length;
    if (temp < 0)
        return temp1;

    return temp;
}

inline bool PointIsConvex(FVector prevPoint, FVector curPoint, FVector nextPoint)
{
    FVector temp1 = prevPoint - curPoint;
    FVector temp2 = nextPoint - curPoint;
    temp1.Normalize();
    temp2.Normalize();
    FVector temp3 = FVector::CrossProduct(temp1, temp2);

    return temp3.Z < 0;
}

inline bool IsPointInTriangle(FVector t1, FVector t2, FVector t3, FVector p)
{
    const FVector v1 = t3 - t1;
    const FVector v2 = t2 - t1;
    const FVector v3 = p - t1;
	
    const float daa = FVector::DotProduct(v1, v1);
    const float dab = FVector::DotProduct(v1, v2);
    const float dac = FVector::DotProduct(v1, v3);
    const float dbb = FVector::DotProduct(v2, v2);
    const float dbc = FVector::DotProduct(v2, v3);

    float u = (((dbb * dac) - (dab * dbc)) / ((daa * dbb) - (dab * dab)));
    float v = (((daa * dbc) - (dab * dac)) / ((daa * dbb) - (dab * dab)));

    return (v >= 0.f) && (u >= 0.f) && ((u + v) < 1.f);
}

inline bool IsPointAnEar(const int curPointIndex, const TArray<int>& reflexIndices, const TArray<FVector>& splinePoints)
{
    bool IsEar = true;

    const int prevIndex = CircularIndex(curPointIndex - 1, splinePoints.Num());
    const int nextIndex = CircularIndex(curPointIndex + 1, splinePoints.Num());

    const FVector curPoint = splinePoints[curPointIndex];
    const FVector prevPoint = splinePoints[prevIndex];
    const FVector nextPoint = splinePoints[nextIndex];

    for (auto i = 0; i < reflexIndices.Num(); i++)
    {
        IsEar = !IsPointInTriangle(curPoint, prevPoint, nextPoint, splinePoints[reflexIndices[i]]);
        if (!IsEar)
            return IsEar;
    }
    return IsEar;
}

void AAreaMesh::CreateArea(const TArray<FVector>& points,int32& area,int32& length,int32& Perimeter)
{
	area=0;
	length=0;
	Perimeter=0;
	Triangles.Reset(0);
	if (points.Num()<2)
		return;		
	if (points.Num()<3) 
	{
		GetLength(points,length);
		return;
	}
	if (IsClockWise(points))
		Points=points;		
	else
		Points=reverseArray(points);
	GetLength(Points,length);
	Getperimeter(Points,Perimeter);
	TArray<MeshTriangle> MeshTriangles;
	TriangulatePoints();
	CreateAreaMesh();
	area=ComputePolygonArea();
	

}
void AAreaMesh::CreateAreaVolume(const TArray<FVector>& points,int32& area,int32& Volume,float Height)
{

	ClearData();
	if (points.Num()<2)
		return;		
	if (points.Num()<3) 
	{
		return;
	}
	    Points=points;
	if (!IsClockWise(points))
		Points=reverseArray(Points);
	
	for (int i=0;i<Points.Num();i++)
	{
		UpPoints.Add(Points[i]+FVector(0,0,Height));
	}
	for (int i=0;i<Points.Num();i++)
	{
		TArray<FVector> temp;
		temp.Add(UpPoints[ CircularIndex(i - 1,Points.Num() )]);
		temp.Add(Points[ CircularIndex(i - 1,Points.Num() )]);
		temp.Add(Points[i]);
		temp.Add(UpPoints[i]);
		SidePointsArrays.Add(temp);
	}

	TArray<MeshTriangle> MeshTriangles;
	TriangulatePointsAsVolume(Points);
	area=ComputePolygonArea();
	Volume=area*Height;
	TriangulatePointsAsVolume(UpPoints);
	for (int i=0;i<points.Num();i++)
	{
		TriangulatePointsAsVolume(SidePointsArrays[i]);
	}
	//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Red,FString::FromInt(Points.Num()));
	CreateAreaMesh();
	//area=ComputePolygonArea();
}
void AAreaMesh::TriangulatePoints()
{
	TArray<FVector> reflexPoints = TArray<FVector>{};
	TArray<FVector> convexPoints = TArray<FVector>{};
	TArray<FVector> earPoints = TArray<FVector>{};
	TArray<int> reflexIndices = TArray<int>{};
	TArray<int> convexIndices = TArray<int>{};
	TArray<int> earIndices = TArray<int>{};
	GetPolygonComponents(Points,reflexPoints,convexPoints,earPoints,reflexIndices,convexIndices,earIndices);
	reflexPoints.Reset();
	convexPoints.Reset();
	earPoints.Reset();
	TrianglesFromPoints(Points,reflexIndices,convexIndices,earIndices);
	
	reflexPoints.Reset();
	convexPoints.Reset();
	earPoints.Reset();
	GetPolygonComponents(UpPoints,reflexPoints,convexPoints,earPoints,reflexIndices,convexIndices,earIndices);
	reflexPoints.Reset();
	convexPoints.Reset();
	earPoints.Reset();
	TrianglesFromPoints(UpPoints,reflexIndices,convexIndices,earIndices);
}

void AAreaMesh::TriangulatePointsAsVolume( TArray<FVector>& splinePoints)
{
	TArray<FVector> reflexPoints = TArray<FVector>{};
	TArray<FVector> convexPoints = TArray<FVector>{};
	TArray<FVector> earPoints = TArray<FVector>{};
	TArray<int> reflexIndices = TArray<int>{};
	TArray<int> convexIndices = TArray<int>{};
	TArray<int> earIndices = TArray<int>{};
	//GetPolygonComponents(Points,reflexPoints,convexPoints,earPoints,reflexIndices,convexIndices,earIndices);
	//GetPolygonComponentsAsVolume(splinePoints,reflexPoints,convexPoints,earPoints,reflexIndices,convexIndices,earIndices,Height);
	GetPolygonComponents(splinePoints,reflexPoints,convexPoints,earPoints,reflexIndices,convexIndices,earIndices);
	reflexPoints.Reset();
	convexPoints.Reset();
	earPoints.Reset();
	TrianglesFromPoints(splinePoints,reflexIndices,convexIndices,earIndices);

}

void AAreaMesh::GetPolygonComponents(const TArray<FVector>& splinePoints, TArray<FVector>& reflexPoints,
                                     TArray<FVector>& convexPoints, TArray<FVector>& earPoints, TArray<int>& reflexIndices, TArray<int>& convexIndices,
                                     TArray<int>& earIndices)
{
	
		FVector prevPoint = FVector();
		FVector nextPoint = FVector();
		for (int curIndex = 0; curIndex < splinePoints.Num(); curIndex++)
		{
			FVector curPoint = splinePoints[curIndex];
			const int prevIndex = CircularIndex(curIndex - 1, splinePoints.Num());
			prevPoint = splinePoints[prevIndex];
			const int nextIndex = CircularIndex(curIndex + 1, splinePoints.Num());
			nextPoint = splinePoints[nextIndex];

			if (PointIsConvex(prevPoint, curPoint, nextPoint))
			{
				convexPoints.Add(curPoint);
				convexIndices.Add(curIndex);

			}
			else
			{
				reflexPoints.Add(curPoint);
				reflexIndices.Add(curIndex);

				
			}
		}
	
		for (int curIndex = 0; curIndex < convexIndices.Num(); curIndex++)
		{
			int curElement = convexIndices[curIndex];

			if (IsPointAnEar(curElement, reflexIndices, splinePoints))
			{
				earPoints.Add(splinePoints[curElement]);
				earIndices.Add(curElement);
			}
		}	
}

void AAreaMesh::GetPolygonComponentsAsVolume(const TArray<FVector>& splinePoints,TArray<FVector>& reflexPoints,
	TArray<FVector>& convexPoints, TArray<FVector>& earPoints, TArray<int>& reflexIndices, TArray<int>& convexIndices,
	TArray<int>& earIndices,float Height)
{
	FVector prevPoint = FVector();
	FVector nextPoint = FVector();
	//底面
	for (int curIndex = 0; curIndex < splinePoints.Num()/2; curIndex++)
	{
		FVector curPoint = splinePoints[curIndex];
		const int prevIndex = CircularIndex(curIndex - 1, splinePoints.Num());
		prevPoint = splinePoints[prevIndex];
		const int nextIndex = CircularIndex(curIndex + 1, splinePoints.Num());
		nextPoint = splinePoints[nextIndex];

		if (PointIsConvex(prevPoint, curPoint, nextPoint))
		{
			convexPoints.Add(curPoint);
			convexIndices.Add(curIndex);

		}
		else
		{
			reflexPoints.Add(curPoint);
			reflexIndices.Add(curIndex);

				
		}
	}

	
	for (int curIndex = 0; curIndex < convexIndices.Num(); curIndex++)
	{
		int curElement = convexIndices[curIndex];

		if (IsPointAnEar(curElement, reflexIndices, splinePoints))
		{
			earPoints.Add(splinePoints[curElement]);
			earIndices.Add(curElement);
		}
	}	
}

void AAreaMesh::TrianglesFromPoints(TArray<FVector>& splinePoints, const TArray<int>& inConvexIndices,
                                    const TArray<int>& inReflexIndices, const TArray<int>& inEarIndices)
{	
	int curPoint = 0;
	if (inEarIndices.Num() > 0)
		curPoint = inEarIndices[0];
	const int prevPoint = CircularIndex(curPoint - 1, splinePoints.Num());
	const int nextPoint = CircularIndex(curPoint + 1, splinePoints.Num());

	if (splinePoints.Num() <= 3)
	{
		MeshTriangle triangle;

			triangle.point1 = splinePoints[0];
			triangle.point2 = splinePoints[1];
			triangle.point3 = splinePoints[2];

		Triangles.Add(triangle);
	}
	else
	{
		MeshTriangle triangle;

			triangle.point1 = splinePoints[curPoint];
			triangle.point2 = splinePoints[prevPoint];
			triangle.point3 = splinePoints[nextPoint];


		splinePoints.RemoveAt(curPoint);

		TArray<FVector> reflexPoints = TArray<FVector>{};
		TArray<FVector> convexPoints = TArray<FVector>{};
		TArray<FVector> earPoints = TArray<FVector>{};
		TArray<int> reflexIndices = TArray<int>{};
		TArray<int> convexIndices = TArray<int>{};
		TArray<int> earIndices = TArray<int>{};
		GetPolygonComponents(splinePoints, reflexPoints, convexPoints, earPoints, reflexIndices, convexIndices,earIndices);
		TrianglesFromPoints(splinePoints, convexIndices, reflexIndices, earIndices);
		Triangles.Add(triangle);
	}
}

void AAreaMesh::TrianglesToIndices(const TArray<MeshTriangle>& triangles, TArray<FVector>& vertices,TArray<int>& indices) const
{
	for (int i = 0; i < triangles.Num(); i++)
	{
		int newIndex = vertices.AddUnique(triangles[i].point1);
		if (newIndex < 0)
			indices.Add(newIndex);
		else
			indices.Add(vertices.Find(triangles[i].point1));

		vertices.AddUnique(triangles[i].point2);
		if (newIndex < 0)
			indices.Add(newIndex);
		else
			indices.Add(vertices.Find(triangles[i].point2));

		vertices.AddUnique(triangles[i].point3);
		if (newIndex < 0)
			indices.Add(newIndex);
		else
			indices.Add(vertices.Find(triangles[i].point3));
	}
}

void AAreaMesh::CreateAreaMesh() const
{

	TArray<FVector> vertices;
	TArray<int> indices;
	TrianglesToIndices(Triangles, vertices, indices);
   
	TArray<FVector> normals;
	for (int i = 0; i < vertices.Num(); i++)
	{
		normals.Add(FVector(0, 0, 1));
	}
	TArray<FVector2D> uv;
	for (int i = 0; i < vertices.Num(); i++)
	{
		uv.Add(FVector2D(0, 0));
	}
	TArray<FProcMeshTangent> tangents;
	for (int i = 0; i < vertices.Num(); i++)
	{
		tangents.Add(FProcMeshTangent(0, 0, 1));
	}
	TArray<FColor> vertexColors;
	for (int i = 0; i < vertices.Num(); i++)
	{
		vertexColors.Add(FColor(0.75, 0.75, 0.75, 1.0));
	}
	pProcedural->ClearMeshSection(0);
	
	pProcedural->CreateMeshSection(0, vertices, indices, normals, uv, vertexColors, tangents, true);
	pProcedural->SetMaterial(0, pAreaMeshMaterial);
}



double AAreaMesh::ComputePolygonArea() const
{
	double s=0;
	for (int index=0;index<Triangles.Num();index++)
	{

		FVector side1=Triangles[index].point1-Triangles[index].point2;
		FVector side2=Triangles[index].point3-Triangles[index].point2;
		s+=FVector::CrossProduct(side1,side2).Size()/2.0;
	}
	return  s;
}

bool AAreaMesh::IsClockWise(const TArray<FVector>& points)
{
	int point_num = points.Num();
	double s=0;
	if(point_num < 3) return 0.0;
     s = points[0].Y * (points[point_num - 1].X - points[1].X);
	for(int i = 1; i < point_num; ++ i)
	{
		s += points[i].Y * (points[i-1].X - points[(i+1)%point_num].X);
	}
	return s+1/2.0>0;
}

void AAreaMesh::GetLength(const TArray<FVector>& points, int32& length)
{

	for (int i=1;i<points.Num();i++)
		length+=(points[i]-points[i-1]).Size();
	/*if (points.Num()>=3)
		Perimeter=length+( points[0]- points[Points.Num()-1]).Size();
	else
		Perimeter=0;*/
		

}

void AAreaMesh::Getperimeter(const TArray<FVector>& points, int32& Perimeter)
{
	if (points.Num()>=3)
	{
		for (int i=1;i<points.Num();i++)
			Perimeter+=(points[i]-points[i-1]).Size();
		Perimeter+=( points[0]- points[Points.Num()-1]).Size();
	}
	else
	Perimeter=0;
}

void AAreaMesh::ClearData()
{
	Triangles.Reset(0);
	Points.Reset(0);
	UpPoints.Reset(0);
	SidePointsArrays.Reset(0);
}