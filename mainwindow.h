#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QMap>
#include <QListWidget>
#include <memory>
#include <QVector>

// 定义家庭成员结构体
struct FamilyMember {
    QString name;  // 成员名称
    QString details;  // 成员详细信息
    QVector<std::shared_ptr<FamilyMember>> children;  // 子节点列表
    QVector<std::shared_ptr<FamilyMember>> spouses;  // 配偶列表（支持多个配偶）
    // 构造函数，用于初始化成员的名称和详细信息
    FamilyMember(const QString& name, const QString& details)
        : name(name), details(details) {}
};
// 定义家庭树类
class FamilyTree {
public:
    FamilyTree();  // 默认构造函数
    explicit FamilyTree(const QString& name);  // 带参数的构造函数，用于初始化家谱名称
    void addMember(const QString& parentName, const QString& name, const QString& details);  // 添加子成员
    void addSpouse(const QString& memberName, const QString& spouseName, const QString& spouseDetails);  // 添加配偶
    void addSibling(const QString& targetName, const QString& siblingName, const QString& siblingDetails);  // 添加兄弟节点
    void modifyMember(const QString& name, const QString& newDetails);  // 修改成员信息
    void modifySpouseDetails(const QString& memberName, const QString& spouseName, const QString& newDetails);  // 修改配偶信息
    void removeSpouse(const QString& memberName, const QString& spouseName);  // 移除配偶
    std::shared_ptr<FamilyMember> findMember(const QString& name);  // 查找成员
    std::shared_ptr<FamilyMember> getRoot() const { return root; }  // 获取家谱的根节点
private:
    QString treeName;  // 家谱名称
    std::shared_ptr<FamilyMember> root;  // 家谱根节点
    std::shared_ptr<FamilyMember> findRecursive(const QString& name, std::shared_ptr<FamilyMember> node);  // 递归查找成员
    std::shared_ptr<FamilyMember> findParent(std::shared_ptr<FamilyMember> currentNode, std::shared_ptr<FamilyMember> targetNode);  // 查找父节点
};

// 定义主窗口类
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddMember();  // 添加成员按钮的槽函数
    void onAddSpouse();  // 添加配偶按钮的槽函数
    void onAddSibling();  // 添加兄弟节点按钮的槽函数
    void onFindMember();  // 查找成员按钮的槽函数
    void onModifyMember();  // 修改成员按钮的槽函数
    void onCreateFamilyTree();  // 创建家谱按钮的槽函数
    void onSwitchFamilyTree();  // 切换家谱按钮的槽函数
    void refreshTree();  // 刷新家谱树视图
    void refreshFamilyTreeList();  // 刷新家谱列表视图
    void removeSpouse(const QString& memberName, const QString& spouseName);  // 移除配偶
    void modifySpouseDetails(const QString& memberName, const QString& spouseName, const QString& newDetails);  // 修改配偶信息
    void onModifySpouseDetails();  // 修改配偶信息按钮的槽函数
    void exportFamilyTreeToCSV();
private:
    Ui::MainWindow *ui;  // UI 界面指针
    QMap<QString, FamilyTree*> familyTrees;  // 家谱映射，保存多个家谱
    FamilyTree* currentFamilyTree;  // 当前选中的家谱
    void addTreeNode(QTreeWidgetItem* parent, std::shared_ptr<FamilyMember> node);  // 添加树节点到界面
    void writeNodeToCSV(QTextStream& out, std::shared_ptr<FamilyMember> node, int level);
};

#endif // MAINWINDOW_H
