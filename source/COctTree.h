#include "terrainclass.h"
#include "CFrustum.h"
#include "CTerrainShader.h"
#include <vector>

class Octree
{

	struct BoundingBox
	{
        float positionX, positionZ, width;
		int triangleCount;
		D3DXVECTOR3 min, max;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
        BoundingBox* nodes[8];
	};

    const int ChildCount = 8;

    ///TO draw the octrees, you provide this (if wanted/needed)
    //static DebugShapesDrawer debugDraw = null;

    //public DebugShapesDrawer DebugDraw
    //{
    //    get { return debugDraw; }
    //    set { debugDraw = value; }
    //}

    D3DXVECTOR3 center;

    float length;
	float looseness;
	int depth;

    BoundingBox bounds;// = default(BoundingBox);

    std::vector<BoundingBox> objects;

    Octree *children;// = null;

    float worldSize;

	/*
    Octree(float worldSize, float looseness, int depth)
        : this(worldSize, looseness, depth, 0, D3DXVECTOR3(0,0,0))
    {
    }

    Octree(float worldSize, float looseness, int depth, D3DXVECTOR3 center)
        : this(worldSize, looseness, depth, 0, center)
    {
    }
	*/

    Octree(float worldSize, float looseness, int maxDepth, int depth, D3DXVECTOR3 center)
    {
		
		looseness = 0;
		depth = 0;
		center = D3DXVECTOR3(0,0,0);
		length = 0.f;
		worldSize = 0.f;
		children = NULL;

        this->worldSize = worldSize;
        this->looseness = looseness;
        this->depth = depth;
        this->center = center;
        this->length = this->looseness * this->worldSize / (float)std::pow(2, this->depth);
        float radius = this->length / 2.f;

        // Create the bounding box.
        D3DXVECTOR3 min = this->center + D3DXVECTOR3(-radius, -radius, -radius);
        D3DXVECTOR3 max = this->center + D3DXVECTOR3(radius, radius, radius);
        //this->bounds = new BoundingBox(min, max);
		this->bounds.min = min;
		this->bounds.max = max;

        // Split the octree if the depth hasn't been reached.
        if (depth < maxDepth)
        {
			return;
            //Split(maxDepth);
        }
    }

    bool Remove(BoundingBox box)
    {
		for(auto &i : objects)
		{
			if(memcmp((void *)(BoundingBox *)&i, (void *)&box, sizeof(BoundingBox)) == 0)
			{
				//objects.erase(i);
				return true;
			}
		}

		printf("OctTree node not found\n");

		return false;
    }
	/*
    bool HasChanged(BoundingBox obj, BoundingBox transformebbox)
    {
        return this->bounds.Contains(transformebbox) == ContainmentType.Contains;
    }

    public bool StillInside(T o, Vector3 center, float radius)
    {
        Vector3 min = center - new Vector3(radius);
        Vector3 max = center + new Vector3(radius);
        BoundingBox bounds = new BoundingBox(min, max);

        if (this.children != null)
            return false;

        if (this.bounds.Contains(bounds) == ContainmentType.Contains)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    public Octree Add(T o, Vector3 center, float radius)
    {
        Vector3 min = center - new Vector3(radius);
        Vector3 max = center + new Vector3(radius);
        BoundingBox bounds = new BoundingBox(min, max);

        if (this.bounds.Contains(bounds) == ContainmentType.Contains)
        {
            return this.Add(o, bounds, center, radius);
        }
        return null;
    }

    public Octree Add(T o, BoundingBox transformebbox)
    {
        float radius = (transformebbox.Max - transformebbox.Min).Length() /2;
        Vector3 center = (transformebbox.Max + transformebbox.Min) / 2;

        if (this.bounds.Contains(transformebbox) == ContainmentType.Contains)
        {
            return this.Add(o, transformebbox, center, radius);
        }
        return null;
    }

    private Octree Add(T o, BoundingBox bounds, Vector3 center, float radius)
    {
        if (this.children != null)
        {
            // Find which child the object is closest to based on where the
            // object's center is located in relation to the octree's center.
            int index = (center.X = this.center.Y ? 0 : 4) +
                (center.Z  objects)
    {
        BoundingFrustum frustum = new BoundingFrustum(view * projection);
        ContainmentType containment = frustum.Contains(this.bounds);

        return this.Draw(frustum, view, projection, containment, objects);
    }

    private int Draw(BoundingFrustum frustum, Matrix view, Matrix projection,
        ContainmentType containment, List objects)
    {
        int count = 0;

        if (containment != ContainmentType.Contains)
        {
            containment = frustum.Contains(this.bounds);
        }

        // Draw the octree only if it is atleast partially in view.
        if (containment != ContainmentType.Disjoint)
        {
            // Draw the octree's bounds if there are objects in the octree.
            if (this.objects.Count > 0)
            {
                if (DebugDraw != null)
                    DebugDraw.AddShape(new DebugBox(this.bounds,Color.White));
                objects.AddRange(this.objects);
                count++;
            }

            // Draw the octree's children.
            if (this.children != null)
            {
                foreach (Octree child in this.children)
                {
                    count += child.Draw(frustum, view, projection, containment, objects);
                }
            }
        }

        return count;
    }

    private void Split(int maxDepth)
    {
        this.children = new Octree[Octree.ChildCount];
        int depth = this.depth + 1;
        float quarter = this.length / this.looseness / 4f;

        this.children[0] = new Octree(this.worldSize, this.looseness,
            maxDepth, depth, this.center + new Vector3(-quarter, quarter, -quarter));
        this.children[1] = new Octree(this.worldSize, this.looseness,
            maxDepth, depth, this.center + new Vector3(quarter, quarter, -quarter));
        this.children[2] = new Octree(this.worldSize, this.looseness,
            maxDepth, depth, this.center + new Vector3(-quarter, quarter, quarter));
        this.children[3] = new Octree(this.worldSize, this.looseness,
            maxDepth, depth, this.center + new Vector3(quarter, quarter, quarter));
        this.children[4] = new Octree(this.worldSize, this.looseness,
            maxDepth, depth, this.center + new Vector3(-quarter, -quarter, -quarter));
        this.children[5] = new Octree(this.worldSize, this.looseness,
            maxDepth, depth, this.center + new Vector3(quarter, -quarter, -quarter));
        this.children[6] = new Octree(this.worldSize, this.looseness,
            maxDepth, depth, this.center + new Vector3(-quarter, -quarter, quarter));
        this.children[7] = new Octree(this.worldSize, this.looseness,
            maxDepth, depth, this.center + new Vector3(quarter, -quarter, quarter));
    }
	*/
};
