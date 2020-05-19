# Range Tree Tester

## Introduction

* Project 2 for [COMP90077](https://handbook.unimelb.edu.au/2020/subjects/comp90077) (Advanced Algorithms and Data Structures) at the University of Melbourne, 2020 Sem1.

* Performance analysis (experimental study) about the [Range Tree](https://en.wikipedia.org/wiki/Range_tree) data structure, including:

1. A data generator, which could generate, given a fixed space, either a random two dimensional data point, or a random two dimensional range query.

2. Two variants of range trees, all support construct and query operations

    a. Original Range Tree: The original range tree, where each internal node u in the base tree (on the x-coordinates) is associated with a secondary WB-BST on the y-coordinates of all the points in u’s sub-tree. The original range tree could be constructed in two ways:
    
        i. Naive construction: the naive construction algorithm that takes O(n log<sup>2</sup> n) time
        
        ii. The O(n log n)-time construction algorithm that takes advantage of sorting.
    
    b. Fractional Cascading Range Tree: incorporating [Fractional Cascading](https://en.wikipedia.org/wiki/Fractional_cascading) as the secondary data structure on each node u, to reduce the O(k + log<sup>2</sup>n) query time to O(k + log n).

3. Experiments about construction efficiency:

    a. Time v.s. Number of data points, length ranged from 10<sup>5</sup> to 10<sup>6</sup>, with universe size being fixed as 10<sup>6</sup>
    
    b. Time v.s. The size of universe (fixed the number data points to be 10<sup>6</sup>, make the size of universe M as 2<sup>i</sup> · 10<sup>3</sup>, where i is an integer from 1 to 10)

4. Experiments about query efficiency:

     a. Time v.s. The range s of query, ranged as s1 = 1%·M, s2 = 2%·M, s3 = 5%·M, s4 = 10%·M and s5 = 20%, with universe size M being fixed as 10<sup>6</sup>
        
     b. Time v.s. The size of universe (fixed the ranged to be 5%·M, make the size of universe M as 2<sup>i</sup> · 10<sup>3</sup>, where i is an integer from 1 to 10)

* For detailed description please check out [project specification](docs/specification.pdf) and [final report](docs/report.pdf)

## Contribution
Xiuge Chen

xiugec@student.unimelb.edu.au

Subject: COMP90077 Advanced Algorithms and Data Structures

University of Melbourne