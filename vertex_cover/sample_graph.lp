\ LP format - for model browsing. Use MPS format to capture full model detail.
Minimize
  vertex0 + vertex1 + vertex2 + vertex3 + vertex4
Subject To
 edge0_2: vertex0 + vertex2 >= 1
 edge1_2: vertex1 + vertex2 >= 1
 edge2_3: vertex2 + vertex3 >= 1
 edge2_4: vertex2 + vertex4 >= 1
 edge3_4: vertex3 + vertex4 >= 1
Bounds
Binaries
 vertex0 vertex1 vertex2 vertex3 vertex4
End
