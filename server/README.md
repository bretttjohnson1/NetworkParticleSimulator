# ParticleSimulator
It simulates interactions between particles. works best with multiple cores, it does physics in parallel.
Currently compiled and tested on Linux.
The Simulation uses a custom data structure "Clusters" to reduce the O(N^2) runtime of naive collision detection to almost O(N) by subdividing the large cube into clusters that count the number of particles. Then it only has to check particles in the surrounding clusters when doing collision detection.

To run:
install muParser
compile/run or just run
