/*using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;

public class COctree
{
    private const int ChildCount = 8;

    private float looseness = 0;

    private int depth = 0;

    ///TO draw the octrees, you provide this (if wanted/needed)
    private static DebugShapesDrawer debugDraw = null;

    public DebugShapesDrawer DebugDraw
    {
        get { return debugDraw; }
        set { debugDraw = value; }
    }

    private Vector3 center = Vector3.Zero;

    private float length = 0f;

    private BoundingBox bounds = default(BoundingBox);

    private List objects = new List();

    private Octree[] children = null;

    private float worldSize = 0f;

    public Octree(float worldSize, float looseness, int depth)
        : this(worldSize, looseness, depth, 0, Vector3.Zero)
    {
    }

    public Octree(float worldSize, float looseness, int depth, Vector3 center)
        : this(worldSize, looseness, depth, 0, center)
    {
    }

    private Octree(float worldSize, float looseness,
        int maxDepth, int depth, Vector3 center)
    {
        this.worldSize = worldSize;
        this.looseness = looseness;
        this.depth = depth;
        this.center = center;
        this.length = this.looseness * this.worldSize / (float)Math.Pow(2, this.depth);
        float radius = this.length / 2f;

        // Create the bounding box.
        Vector3 min = this.center + new Vector3(-radius);
        Vector3 max = this.center + new Vector3(radius);
        this.bounds = new BoundingBox(min, max);

        // Split the octree if the depth hasn't been reached.
        if (this.depth < maxDepth)
        {
            this.Split(maxDepth);
        }
    }

    public void Remove(T obj)
    {
        objects.Remove(obj);
    }

    public bool HasChanged(T obj, BoundingBox transformebbox)
    {
        return this.bounds.Contains(transformebbox) == ContainmentType.Contains;
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

}
*/