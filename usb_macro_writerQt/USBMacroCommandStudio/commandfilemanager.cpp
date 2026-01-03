#include "commandfilemanager.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

bool CommandFileManager::saveToFile(const QString &path,
                                    const QList<CommandRecord> &commands)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "[CommandFileManager] Failed to open file for write:" << path;
        return false;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif

    // 포맷:  type <TAB> cmd <NEWLINE>
    for (const CommandRecord &rec : commands) {
        // 안전장치: type 없는 경우는 스킵
        if (rec.type == '\0')
            continue;

        out << rec.type << '\t' << rec.cmd << '\n';
    }

    file.close();
    return true;
}

bool CommandFileManager::loadFromFile(const QString &path,
                                      QList<CommandRecord> &commands)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "[CommandFileManager] Failed to open file for read:" << path;
        return false;
    }

    QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    in.setCodec("UTF-8");
#endif

    commands.clear();

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty())
            continue;

        // 기대 포맷:  "T<TAB>cmd..."
        // 최소 길이: type(1) + tab(1) = 2
        if (line.size() < 2 || line[1] != '\t')
            continue;   // 잘못된 라인 스킵 (원하면 qDebug로 찍어도 됨)

        CommandRecord rec;
        rec.type = line[0].toLatin1();
        rec.cmd  = line.mid(2);      // 탭 뒤부터 끝까지

        commands.append(rec);
    }

    file.close();
    return true;
}
