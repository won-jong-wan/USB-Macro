# Visual Command Editor (비주얼 커맨드 에디터)

장치 제어 명령어를 리스트 형태로 작성하고, 이를 **노드 그래프(Flowchart)** 형태로 실시간 시각화하여 관리하는 Qt 기반 데스크톱 애플리케이션입니다.

<img width="1181" height="697" alt="image" src="https://github.com/user-attachments/assets/04ba0c8f-e81e-449b-b11b-edc559eb555b" />


## ✨ Key Features (주요 기능)
* **명령어 셋 편집**: 명령어 셋을 편집하고 타입(c, s, d)을 지정
* **실시간 노드 시각화**: 리스트에 입력된 명령어를 타입에 맞춰 시각적 노드로 변환
* **장치 전송**: 명령어들을 패킷 구조체로 변환하여 장치에 write
* **장치 일기**: 장치에 저장된 명령어 셋을 다운로드
* **명령어 셋 저장**: 명령어 셋을 pc에 저장하고 불러오는 기능

## 🛠 Tech Stack

* **Language**: C++
* **Framework**: Qt (Widgets, Graphics View)
* **Tools**: Qt Creator, qmake


## 🏗 Structure (Class Diagram)
```mermaid
classDiagram
    %% 메인 윈도우: 전체 관리
    class MainWindow {
        -Ui::MainWindow* ui
        -GridScene* m_scene
        -NodeManager* m_nodeManager
        +MainWindow(parent)
        +update_view()
        +on_saveBtn_clicked()
        +on_loadBtn_clicked()
    }

    %% 노드 매니저: 로직과 뷰의 중개자
    class NodeManager {
        -GridScene* m_scene
        -QList~CommandNode*~ m_nodes
        -QMap~CommandNode*, VisualNode*~ m_visualMap
        -CommandNode* m_lastMainNode
        +addCommand(NodeType, QString)
        +clear()
        +updateSceneRect()
    }

    %% 그래픽 씬: 배경 및 아이템 컨테이너
    class GridScene {
        +drawBackground(painter, rect)
    }

    %% 비주얼 노드: 실제 화면에 그려지는 박스
    class VisualNode {
        -CommandNode* m_data
        -QList~Edge*~ m_edges
        -qreal m_width
        -qreal m_height
        +paint(painter, option, widget)
        +itemChange(change, value)
        +addEdge(Edge*)
    }

    %% 커맨드 노드: 순수 데이터 (Type, Text)
    class CommandNode {
        -NodeType m_type
        -QString m_text
        +getType()
        +getText()
    }

    %% 엣지: 노드 간 연결선
    class Edge {
        -VisualNode* sourceNode
        -VisualNode* destNode
        -bool m_isBranch
        +adjust()
        +paint(painter, option, widget)
    }

    %% 유틸리티 클래스
    class CommandFileManager {
        <<Utility>>
        +saveToFile()
        +loadFromFile()
    }

    class WonDeviceApi {
        <<Utility>>
        +createPacket()
    }

    %% 관계 정의 (Relationships)
    MainWindow --> NodeManager : Creates & Manages
    MainWindow --> GridScene : Has
    MainWindow ..> CommandFileManager : Uses
    MainWindow ..> WonDeviceApi : Uses

    NodeManager o-- CommandNode : Aggregates
    NodeManager --> VisualNode : Creates
    NodeManager ..> Edge : Creates
    NodeManager --> GridScene : Adds Items To

    VisualNode --> CommandNode : Wraps Data
    VisualNode "1" -- "*" Edge : Connected

    Edge --> VisualNode : Links
    
    GridScene o-- VisualNode : Contains
    GridScene o-- Edge : Contains
```
