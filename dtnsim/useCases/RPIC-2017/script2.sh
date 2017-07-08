#!/bin/bash

python3 deletedContactsAnalysis_centrality_0.1.py
python3 deletedContactsAnalysis_centrality_0.5.py
python3 deletedContactsAnalysis_centrality_1.0.py

python3 deletedContactsAnalysis_random_0.1.py
python3 deletedContactsAnalysis_random_0.5.py
python3 deletedContactsAnalysis_random_1.0.py

python3 NCurvas_recibidos_centrality.py
python3 NCurvas_reruteados_centrality.py
python3 NCurvas_rutas_disponibles_centrality.py
python3 NCurvas_nodos_conectados_centrality.py

python3 NCurvas_recibidos_random.py
python3 NCurvas_reruteados_random.py
python3 NCurvas_rutas_disponibles_random.py
python3 NCurvas_nodos_conectados_random.py

: <<'END'
END
