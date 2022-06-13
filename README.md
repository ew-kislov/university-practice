# Moscow State University practice

Here I post all the practices I've completed during studying in Moscow State University, department of Computational Mathematics and Cybernetics.

## Semester 4

- Formal languages ([source code](4-formal-languages-practice))  
  I have developed light Data Base Management System, including query parser and DB engine.  
  Supported types of queries and data types you can find in the practice's [README.md](4-formal-languages-practice/README.md)).

## Semester 5

- Parallel practice ([source code](5-parallel-practice))  
  This practice was dedicated to implementing various parallel algorithms using OpenMPI, OpenMP, POSIX threads and researching effeciency of parallelizing.

## Semester 6

- OpenGL practice ([source code](6-opengl-practice))  
  I have developed 2D engine with range of facilities - animation, UI system, resources upload system, map loader, etc.  
  I have also developed 2D game using my engine. You can find video of gameplay in the practice's [README.md](6-opengl-practice/README.md)).  

- Ray tracing practice ([source code](6-ray-tracing-practice))  
  I have developed ray tracer which uses path tracing technique. The project doesn't use third-party libraries, all math and other operations were implemented by myself.  
  Ray tracer was optimized by parallelizing cast of rays into the same point.  
  You can find example of rendered image in the practice's [README.md](6-ray-tracing-practice/README.md)).  

## Semester 7

- CUDA practice ([source code](7-cuda-practice))  
  This practice was dedicated to implementing image modification by applying kernel. Then, program was optimized by appying sush techniques as: shared memory, warps, unrolling loops, etc.  

- Quantum practice ([source code](7-quantum-practice))  
  This practice was dedicated to implementing various quantum algorithms and parallelizing them using OpenMP and OpenMPI.  

- Distributed systems practice using RabbitMQ ([part 1](7-rabbitmq-practice-1), [part 2](7-rabbitmq-practice-2), [part 3](7-rabbitmq-practice-3))  
  The goal was to implement various distributed approaches using messaging: worker pool, broadcasting techniques, algorithms using logical(Lamport) clock.  

## Semester 8

- Graph generator practice ([source code](7-cuda-practice))  
  I have implemented parallel graph generation, applying jobs pool approach and using std::threadsfor parallel computing.

## Other works

- Graph generation algorithms ([source code](other-graph-generation-algorithms))  
  This practice was dedicated to researching various graph generation algorithms and research of possibilities or their parallelization.  

## Graduade work - "Jobs Scheduler for Lomonosov-2 Supercomputer" ([source code](graduate-work))  
  I have implemented jobs scheduler for my university supercomputer and put it into production environment.
  As a result, my scehduler showed 20% effeciency compared to previous job scheduler working on supercomputer Lomonosov-2.  
