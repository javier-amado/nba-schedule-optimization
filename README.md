This repository includes the files described in Appendix A of the report, developed as part of Javier Amado Lázaro's Bachelor Thesis:
The NBA Schedule: Optimization Models and Metaheuristics.

Planning matches in professional sports is a complex task that impacts both the health of
players and the economic, logistical, and media aspects of the competition. This Final Degree
Project focuses on designing an optimised schedule for the NBA regular season, with the main
objective of minimising the distances travelled by teams throughout the competitive season. In
this context, the well-being of athletes is prioritised, seeking a significant reduction in travel to
decrease accumulated fatigue.

First, an analysis of the functioning of the official NBA calendar is carried out, identifying
key restrictions and rules through a literature review. Subsequently, an exact mathematical
model capable of representing these restrictions is developed, which is solved using the Gurobi
optimiser.

However, due to the high complexity of the problem, heuristic algorithms (exchange of days,
change of home-away games, etc.) and metaheuristic algorithms (Simulated Annealing, VNS and
Tabu Search) are implemented to obtain approximate solutions in reasonable computing times,
efficiently exploring the solution space.

As a result, a tool has been developed that combines exact generation with heuristic optimi-
sation, obtaining solutions that represent a substantial improvement in total travel compared
to the official schedule. All of this has been done with a modular approach, allowing for future
expansion and adaptation to other contexts.
