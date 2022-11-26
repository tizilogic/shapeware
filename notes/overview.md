# Overview of the project

## Scope for initial version

### Must have

Create a generic library that allows the user to:
1. Create complex [SDF](https://en.wikipedia.org/wiki/Signed_distance_function)s using provided
geometric primitives, [CSG](https://en.wikipedia.org/wiki/Constructive_solid_geometry) operations
and modifiers (Vertex colors only)
1. Extract a mesh using the [Marching Cubes](https://en.wikipedia.org/wiki/Marching_cubes) algorithm
   - Optionally optimize the mesh *(e.g. Vertex deduplication)*
2. Generate vertex + index buffer from a mesh


### Don't need

- Animating, Skeleton, Joints and similar
- Serialization
- Materials

------

## Workflow abstract

1. Specify `part` designs and combine multiple `parts` to a `model`
2. Extract surface using the Adaptive [Marching Cubes](https://en.wikipedia.org/wiki/Marching_cubes)
algorithm
3. *Optional* Deduplicate vertices
4. Generate and expose vertex + index buffer pair for use in 3D rendering

------

## Terms

|Identifier|Description|
|-|-|
|[Primitive](https://en.wikipedia.org/wiki/Geometric_primitive)|-|
|[CSG](https://en.wikipedia.org/wiki/Constructive_solid_geometry)|-|
|[Marching Cubes](https://en.wikipedia.org/wiki/Marching_cubes)|-|
|Part|Describes a combination of `primitives` "combined" using `CSG` operations|
|Model|Describes a combination of `parts` from which a mesh will be extracted|
|Modifiers|Transforms, rounding, etc.|

------

## Exposed API layout

- Types
  - Primitive
  - CSG ops
  - Modifiers
  - Part
  - Model
  - Mesh
  - Vertex buffer
  - Index buffer
- Functions
  -
