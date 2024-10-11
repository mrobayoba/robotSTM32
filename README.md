# robotSTM32
 robotics projects with STM32

 - robotRTOS project works in a differential wheeled robot with a PID for each wheel with encoder counting as feedback and Astar to calculate the path to travel. The map (max_size=16x16) to be used with Astar can be given as a string where 'O' means walkable tile, '#' is obstacle, 'S' for start tile and G for goal tile. Use % as row-end character. Type 'help' by serial to see all available options. This is a prototype. This project use a STM32 F411RE Nucleo Board.
