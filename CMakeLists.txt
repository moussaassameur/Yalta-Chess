cmake_minimum_required(VERSION 3.16)
project(YaltaChess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ─── Sources cumulatives jusqu'a la Phase courante ───────────────────────

set(SOURCES_MOTEUR
    # Phase 1
    src/model/Case.cpp
    src/model/Plateau.cpp
    # Phase 2 (Pattern Command)
    src/model/Historique.cpp
    src/coup/CoupSimple.cpp
    src/coup/CoupPromotion.cpp
    src/coup/CoupRoque.cpp
    src/coup/CoupPriseEnPassant.cpp
    # Phase 3 (Pieces)
    src/pieces/Piece.cpp
    src/pieces/Pion.cpp
    # Phase 4 (autres pieces)
    src/pieces/Tour.cpp
    src/pieces/Cavalier.cpp
    src/pieces/Fou.cpp
    src/pieces/Reine.cpp
    src/pieces/Roi.cpp
    # Phase 5 (Observer + ModeleJeu)
    src/joueur/Joueur.cpp
    src/joueur/JoueurHumain.cpp
    src/model/ModeleJeu.cpp
    # Phase 7 (IA MinMax)
    src/ia/MinMax.cpp
)

set(HEADERS_MOTEUR
    include/model/Couleur.hpp
    include/model/Case.hpp
    include/model/Plateau.hpp
    include/model/Historique.hpp
    include/coup/Coup.hpp
    include/coup/CoupSimple.hpp
    include/coup/CoupPromotion.hpp
    include/coup/CoupRoque.hpp
    include/coup/CoupPriseEnPassant.hpp
    include/pieces/Piece.hpp
    include/pieces/Pion.hpp
    include/pieces/Tour.hpp
    include/pieces/Cavalier.hpp
    include/pieces/Fou.hpp
    include/pieces/Reine.hpp
    include/pieces/Roi.hpp
    include/observer/Observateur.hpp
    include/model/EtatPartie.hpp
    include/joueur/Joueur.hpp
    include/joueur/JoueurHumain.hpp
    include/model/ModeleJeu.hpp
    include/ia/MinMax.hpp
)

# ─── Tests console (sans Qt) ─────────────────────────────────────────────

foreach(test_name IN ITEMS
        test_phase1 test_phase2 test_phase3
        test_tour test_cavalier test_fou test_reine test_roi
        test_phase5 test_phase6_init test_roque test_en_passant test_minmax)
    add_executable(${test_name}
        tests/${test_name}.cpp
        ${SOURCES_MOTEUR}
        ${HEADERS_MOTEUR}
    )
    target_include_directories(${test_name} PRIVATE include)
endforeach()

# ─── Application GUI Qt (Phase 6) ────────────────────────────────────────

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(SOURCES_GUI
    src/view/VueJeu.cpp
    src/controller/ControleurJeu.cpp
)
set(HEADERS_GUI
    include/view/VueJeu.hpp
    include/controller/ControleurJeu.hpp
)

add_executable(YaltaChess
    main.cpp
    ${SOURCES_MOTEUR}
    ${HEADERS_MOTEUR}
    ${SOURCES_GUI}
    ${HEADERS_GUI}
)
target_include_directories(YaltaChess PRIVATE include)
target_link_libraries(YaltaChess PRIVATE Qt6::Core Qt6::Widgets)
