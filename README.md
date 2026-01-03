# Visual Command Editor (비주얼 커맨드 에디터)

장치 제어 명령어를 리스트 형태로 작성하고, 이를 **노드 그래프(Flowchart)** 형태로 실시간 시각화하여 관리하는 Qt 기반 데스크톱 애플리케이션입니다.

![Screenshot]([여기에_스크린샷_이미지_경로.png])

## ✨ Key Features (주요 기능)

* **Real-time Visualization**: 리스트에 입력된 명령어를 Start, Check, Delay 등 타입에 맞춰 시각적 노드로 변환합니다.
* **Auto Layout Algorithm**:
    * 순차 실행(Sequence)과 조건 분기(Branch)를 구분하여 노드를 자동으로 배치합니다.
    * 'Ghost Node' 알고리즘을 적용하여 복잡한 분기 상황에서도 그래프 형태가 무너지지 않습니다.
* **Smart Interactions**:
    * Qt Graphics View Framework 기반으로 부드러운 줌/팬(Zoom/Pan)을 지원합니다.
    * 노드를 드래그하면 연결된 엣지(Edge)가 자동으로 경로를 재계산하여 따라옵니다.
* **Packet Generation**: 작성된 시나리오를 하드웨어 통신용 바이너리 패킷으로 변환합니다 (`WonDeviceApi`).
* **Save/Load**: 작업 내용을 `.wcmd` 포맷으로 저장하고 불러올 수 있습니다.

## 🛠 Tech Stack

* **Language**: C++
* **Framework**: Qt (Widgets, Graphics View)
* **Tools**: Qt Creator, qmake/cmake

## 🏗 Structure (Class Diagram)

```mermaid
classDiagram
    class MainWindow {
        +update_view()
    }
    class NodeManager {
        +addCommand()
        +clear()
    }
    class VisualNode {
        +paint()
        +itemChange()
    }
    class Edge {
        +adjust()
    }
    
    MainWindow --> NodeManager : Manages
    NodeManager --> VisualNode : Creates
    NodeManager --> Edge : Connects
    VisualNode "1" -- "*" Edge : Linked
