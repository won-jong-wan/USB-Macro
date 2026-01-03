# Visual Command Editor (비주얼 커맨드 에디터)

장치 제어 명령어를 리스트 형태로 작성하고, 이를 **노드 그래프(Flowchart)** 형태로 실시간 시각화하여 관리하는 Qt 기반 데스크톱 애플리케이션입니다.

<img width="1181" height="697" alt="image" src="https://github.com/user-attachments/assets/04ba0c8f-e81e-449b-b11b-edc559eb555b" />


## ✨ 주요 기능 (Key Features)

* **✏️ 명령어 셋 편집 (Command Editing)**
    * 리스트 위젯을 통한 직관적인 명령어 추가 및 수정
    * 속성별 타입(S, C, D) 지정을 통한 로직 구분

* **📊 실시간 노드 시각화 (Real-time Visualization)**
    * 텍스트 명령어 리스트의 즉각적인 노드 플로우차트 변환
    * 분기(Check) 및 지연(Delay) 등 로직 흐름의 시각적 확인

* **📡 장치 제어 패킷 전송 (Device Control - Write)**
    * 명령어 셋을 하드웨어 프로토콜 맞춤형 패킷으로 변환
    * 시리얼 통신 등을 이용한 장치(Device)로의 명령 업로드

* **📥 장치 데이터 수신 (Device Sync - Read)**
    * 장치 내 저장된 기존 명령어 셋 다운로드
    * 하드웨어와 소프트웨어 간 데이터 동기화 지원

* **💾 프로젝트 저장 및 불러오기 (File Persistence)**
    * 전용 포맷(`.wcmd`)을 이용한 작업 내역 저장 및 로드
    * 데이터 이식성 확보 및 백업 지원
 

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
