#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
// 默认构造函数，初始化家谱树时设置根节点为空
FamilyTree::FamilyTree() : root(nullptr) {}
// 带名称的构造函数，用于创建一个命名的家谱树，并初始化根节点为空
FamilyTree::FamilyTree(const QString& name) : treeName(name), root(nullptr) {}

// 修改配偶详细信息
// 输入：成员名称、配偶名称、新的配偶详细信息
void FamilyTree::modifySpouseDetails(const QString& memberName, const QString& spouseName, const QString& newDetails) {
    // 查找指定的成员
    auto member = findMember(memberName);
    if (member) {
        // 遍历成员的配偶列表
        for (auto& spouse : member->spouses) {
            // 如果找到匹配的配偶，更新其详细信息
            if (spouse->name == spouseName) {
                spouse->details = newDetails; // 更新配偶详细信息
                qDebug() << "成功修改配偶信息: " << spouseName << " -> " << newDetails;
                return;
            }
        }
        // 如果未找到配偶，输出日志信息
        qDebug() << "未找到配偶: " << spouseName << " -> 属于成员: " << memberName;
    } else {
        // 如果未找到成员，输出日志信息
        qDebug() << "未找到成员: " << memberName;
    }
}

// 添加成员到家谱树
// 输入：父节点名称（如果为空表示添加根节点）、新成员名称、新成员详细信息
void FamilyTree::addMember(const QString& parentName, const QString& name, const QString& details) {
    qDebug() << "Attempting to add member: " << name << " with parent: " << (parentName.isEmpty() ? "ROOT" : parentName);

    // 创建新的家庭成员对象
    auto newMember = std::make_shared<FamilyMember>(name, details);

    if (parentName.isEmpty()) {
        // 如果父节点名称为空，检查是否存在根节点
        if (!root) {
            // 如果根节点不存在，将新成员设置为根节点
            root = newMember;
            qDebug() << "Root member added: " << name;
        } else {
            // 如果根节点已存在，输出提示信息
            qDebug() << "Root member already exists! Current root: " << root->name;
        }
    } else {
        // 查找指定的父节点
        auto parent = findMember(parentName);
        if (parent) {
            // 如果找到父节点，将新成员添加为其子节点
            parent->children.append(newMember);
            qDebug() << "Child member added: " << name << " to parent: " << parentName;
        } else {
            // 如果未找到父节点，输出提示信息
            qDebug() << "Parent not found! ParentName: " << parentName;
            qDebug() << "Check if the parent name matches the root node or other nodes exactly.";
        }
    }
}

void FamilyTree::addSpouse(const QString& memberName, const QString& spouseName, const QString& spouseDetails) {
    // 查找目标成员
    auto member = findMember(memberName);
    if (member) {
        // 检查配偶列表中是否已经存在该配偶
        for (const auto& spouse : member->spouses) {
            if (spouse->name == spouseName) {
                // 如果配偶已存在，输出提示信息并返回
                qDebug() << "配偶已存在: " << spouseName;
                return;
            }
        }

        // 创建一个新配偶对象
        auto newSpouse = std::make_shared<FamilyMember>(spouseName, spouseDetails);

        // 将新配偶添加到成员的配偶列表中
        member->spouses.append(newSpouse);

        // 建立双向关联：将当前成员添加到新配偶的配偶列表中
        newSpouse->spouses.append(member);

        // 输出成功添加配偶的信息
        qDebug() << "成功添加配偶: " << spouseName << " -> " << memberName;
    } else {
        // 如果未找到目标成员，输出提示信息
        qDebug() << "未找到成员: " << memberName;
    }
}
void FamilyTree::modifyMember(const QString& name, const QString& newDetails) {
    // 调用 findMember 方法查找指定名称的成员
    auto member = findMember(name);
    if (member) {
        // 如果成员找到，更新其详细信息
        member->details = newDetails;
        qDebug() << "Successfully updated member details for: " << name;
    } else {
        // 如果未找到成员，输出错误信息
        qDebug() << "Member not found!";
    }
}

std::shared_ptr<FamilyMember> FamilyTree::findMember(const QString& name) {
    // 从根节点开始递归查找成员
    return findRecursive(name, root);
}

std::shared_ptr<FamilyMember> FamilyTree::findRecursive(const QString& name, std::shared_ptr<FamilyMember> node) {
    if (!node) {
        // 如果当前节点为空，返回 nullptr
        qDebug() << "Node is null, returning nullptr";
        return nullptr;
    }

    // 输出日志信息，显示当前检查的节点
    qDebug() << "Checking node: " << node->name;

    // 如果找到匹配的节点，返回该节点
    if (node->name == name) {
        qDebug() << "Found matching node: " << node->name;
        return node;
    }

    // 遍历当前节点的所有子节点
    for (const auto& child : node->children) {
        // 输出日志信息，表示正在检查子节点
        qDebug() << "Descending into child: " << child->name;

        // 递归调用 findRecursive 检查子节点
        auto found = findRecursive(name, child);
        if (found) {
            return found; // 如果找到匹配的节点，则返回
        }
    }

    // 如果在当前分支中未找到目标节点，输出日志信息
    qDebug() << "Node not found in this branch: " << name;
    return nullptr; // 未找到匹配的节点，返回 nullptr
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentFamilyTree(nullptr) // 初始化当前家谱树为 nullptr
{
    ui->setupUi(this); // 设置 UI 组件

    // 按钮信号与槽函数的连接
    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::onCreateFamilyTree); // 创建家谱按钮
    connect(ui->switchButton, &QPushButton::clicked, this, &MainWindow::onSwitchFamilyTree); // 切换家谱按钮
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddMember);           // 添加成员按钮
    connect(ui->findButton, &QPushButton::clicked, this, &MainWindow::onFindMember);         // 查找成员按钮
    connect(ui->modifyButton, &QPushButton::clicked, this, &MainWindow::onModifyMember);     // 修改成员按钮
    connect(ui->addSpouseButton, &QPushButton::clicked, this, &MainWindow::onAddSpouse);     // 添加配偶按钮
    connect(ui->addSiblingButton, &QPushButton::clicked, this, &MainWindow::onAddSibling);   // 添加兄弟节点按钮
    connect(ui->modifySpouseButton, &QPushButton::clicked, this, &MainWindow::onModifySpouseDetails); // 修改配偶信息按钮
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::exportFamilyTreeToCSV);
    // 设置树形组件的样式和列宽
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::Stretch); // 设置列宽自动调整
    ui->treeWidget->setStyleSheet("background:transparent;"); // 设置背景透明
    ui->familyTreeList->setStyleSheet("background:transparent;"); // 设置家谱列表背景透明

    // 设置占位符文本，提示用户输入
    ui->nameEdit->setPlaceholderText(QString::fromUtf8("在这里输入子节点")); // 子节点输入框
    ui->parentEdit->setPlaceholderText(QString::fromUtf8("在这里输入根节点")); // 父节点输入框
    ui->detailsEdit->setPlaceholderText(QString::fromUtf8("在这里输入性别、信息")); // 详细信息输入框
    ui->familyNameEdit->setPlaceholderText(QString::fromUtf8("请先输入家谱名称")); // 家谱名称输入框

    // 双击家谱列表切换家谱
    connect(ui->familyTreeList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        QString familyName = item->text(); // 获取双击的家谱名称
        if (familyTrees.contains(familyName)) {
            currentFamilyTree = familyTrees[familyName]; // 切换到指定家谱
            qDebug() << "Switched to family tree:" << familyName; // 输出日志信息
            refreshTree(); // 刷新当前家谱树
        } else {
            qDebug() << "Family tree not found!"; // 未找到家谱的提示
        }
    });
}

MainWindow::~MainWindow() {
    delete ui; // 删除 UI 组件
    qDeleteAll(familyTrees); // 删除所有家谱对象，释放内存
}

void MainWindow::refreshFamilyTreeList() {
    ui->familyTreeList->clear(); // 清空列表
    for (const auto& familyName : familyTrees.keys()) {
        ui->familyTreeList->addItem(familyName); // 添加家谱名称到列表中
    }
}

void MainWindow::onCreateFamilyTree() {
    QString familyName = ui->familyNameEdit->text().trimmed(); // 去除空格
    if (familyName.isEmpty()) {
        qDebug() << "Family name cannot be empty!";
        return;
    }

    if (familyTrees.contains(familyName)) {
        qDebug() << "Family tree already exists!";
    } else {
        auto newTree = new FamilyTree(familyName);
        familyTrees[familyName] = newTree;
        currentFamilyTree = newTree;

        // 添加根节点
        currentFamilyTree->addMember("", familyName, ""); // 默认以家谱名称作为根节点
        qDebug() << "Created family tree: " << familyName;

        refreshTree();
        refreshFamilyTreeList(); // 刷新家谱列表
    }
}


void MainWindow::onSwitchFamilyTree() {
    QString familyName = ui->familyNameEdit->text();
    if (familyTrees.contains(familyName)) {
        currentFamilyTree = familyTrees[familyName];
        qDebug() << "Switched to family tree:" << familyName;
        refreshTree();

        // 更新列表中的选中状态
        QList<QListWidgetItem*> items = ui->familyTreeList->findItems(familyName, Qt::MatchExactly);
        if (!items.isEmpty()) {
            ui->familyTreeList->setCurrentItem(items.first());
        }
    } else {
        qDebug() << "Family tree not found!";
    }
}

// 添加成员
void MainWindow::onAddMember() {
    if (!currentFamilyTree) {
        QMessageBox::warning(this, "警告", "请先选择或创建一个家谱！");
        return;
    }

    QString parentName = ui->parentEdit->text().trimmed();
    QString name = ui->nameEdit->text().trimmed();
    QString details = ui->detailsEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "子节点名称不能为空！");
        return;
    }

    currentFamilyTree->addMember(parentName, name, details);
    QMessageBox::information(this, "成功", "成功添加成员：" + name);
    refreshTree(); // 刷新树视图
}
// 寻找成员
// 寻找成员
void MainWindow::onFindMember() {
    if (!currentFamilyTree) {
        QMessageBox::warning(this, "警告", "请先选择或创建一个家谱！");
        return;
    }

    QString name = ui->nameEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入要寻找的成员名称！");
        return;
    }

    auto member = currentFamilyTree->findMember(name);
    if (member) {
        QString info = "找到成员：\n";
        info += "名称：" + member->name + "\n";
        info += "详细信息：" + member->details + "\n";

        // 遍历配偶列表
        if (!member->spouses.isEmpty()) {
            info += "配偶：\n";
            for (const auto& spouse : member->spouses) {
                info += " - 名称：" + spouse->name + "\n";
                info += "   详细信息：" + spouse->details + "\n";
            }
        } else {
            info += "无配偶信息\n";
        }

        QMessageBox::information(this, "找到成员", info);
    } else {
        QMessageBox::warning(this, "未找到", "未找到成员：" + name);
    }
}
void MainWindow::onModifyMember() {
    if (!currentFamilyTree) {
        QMessageBox::warning(this, "错误", "尚未选择家谱！");
        return;
    }

    QString memberName = ui->nameEdit->text().trimmed(); // 当前成员名称
    QString newDetails = ui->detailsEdit->text().trimmed(); // 新的详细信息
    QString spouseName = ui->parentEdit->text().trimmed(); // 可选的配偶名称

    if (memberName.isEmpty() || newDetails.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "成员名称和新信息不能为空！");
        return;
    }

    // 修改当前成员信息
    auto member = currentFamilyTree->findMember(memberName);
    if (member) {
        // 修改成员信息
        member->details = newDetails;
        QMessageBox::information(this, "操作成功", QString("成员 %1 信息已修改为: %2").arg(memberName).arg(newDetails));
    } else {
        QMessageBox::warning(this, "错误", QString("未找到成员: %1").arg(memberName));
        return;
    }

    // 修改配偶信息
    if (!spouseName.isEmpty()) {
        bool spouseFound = false;
        for (auto& spouse : member->spouses) {
            if (spouse->name == spouseName) {
                spouse->details = newDetails;
                QMessageBox::information(this, "操作成功", QString("配偶 %1 的信息已修改为: %2").arg(spouseName, newDetails));
                spouseFound = true;
                break;
            }
        }
        if (!spouseFound) {
            QMessageBox::warning(this, "错误", QString("成员 %1 的配偶中未找到: %2").arg(memberName, spouseName));
        }
    }

    // 刷新家谱树视图
    refreshTree();
}
void MainWindow::addTreeNode(QTreeWidgetItem* parent, std::shared_ptr<FamilyMember> node) {
    if (!node) return;

    QString displayText = node->name;

    // 添加所有配偶信息
    if (!node->spouses.isEmpty()) {
        displayText += " (配偶: ";
        for (int i = 0; i < node->spouses.size(); ++i) {
            displayText += node->spouses[i]->name;
            if (i < node->spouses.size() - 1) {
                displayText += ", ";
            }
        }
        displayText += ")";
    }

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, displayText); // 显示成员信息
    item->setText(1, node->details); // 显示成员详细信息

    // 显示配偶的详细信息
    if (!node->spouses.isEmpty()) {
        QString spouseDetails;
        for (const auto& spouse : node->spouses) {
            spouseDetails += spouse->name + ": " + spouse->details + "\n";
        }
        item->setText(2, spouseDetails.trimmed()); // 配偶信息列
    }

    if (parent) {
        parent->addChild(item);
    } else {
        ui->treeWidget->addTopLevelItem(item);
    }

    // 遍历子节点
    for (const auto& child : node->children) {
        addTreeNode(item, child);
    }
}

void MainWindow::onAddSpouse() {
    if (!currentFamilyTree) {
        QMessageBox::warning(this, "错误", "尚未选择家谱！");
        return;
    }

    QString memberName = ui->parentEdit->text().trimmed(); // 成员名称
    QString spouseName = ui->nameEdit->text().trimmed();   // 配偶名称
    QString spouseDetails = ui->detailsEdit->text().trimmed(); // 配偶详细信息

    if (memberName.isEmpty() || spouseName.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "成员名称和配偶名称不能为空！");
        return;
    }

    currentFamilyTree->addSpouse(memberName, spouseName, spouseDetails);
    QMessageBox::information(this, "操作成功", QString("成功为成员 %1 添加配偶 %2").arg(memberName, spouseName));

    refreshTree(); // 刷新视图
}



void MainWindow::refreshTree() {
    ui->treeWidget->clear(); // 清空树视图
    if (currentFamilyTree && currentFamilyTree->getRoot()) {
        qDebug() << "Refreshing tree from root: " << currentFamilyTree->getRoot()->name;
        addTreeNode(nullptr, currentFamilyTree->getRoot()); // 从根节点开始递归添加
    } else {
        qDebug() << "No root node to refresh!";
    }
}

void FamilyTree::addSibling(const QString& targetName, const QString& siblingName, const QString& siblingDetails) {
    // 查找目标节点
    auto targetNode = findMember(targetName);
    if (!targetNode) {
        qDebug() << "Target node not found for adding sibling: " << targetName;
        return;
    }

    // 查找父节点
    auto parentNode = findParent(root, targetNode);
    if (!parentNode) {
        qDebug() << "Parent node not found for target: " << targetName;
        return;
    }

    // 创建新的兄弟节点并添加到父节点的子节点列表
    auto newSibling = std::make_shared<FamilyMember>(siblingName, siblingDetails);
    parentNode->children.append(newSibling);
    qDebug() << "Sibling added: " << siblingName << " to parent: " << parentNode->name;
}
std::shared_ptr<FamilyMember> FamilyTree::findParent(std::shared_ptr<FamilyMember> currentNode, std::shared_ptr<FamilyMember> targetNode) {
    if (!currentNode) return nullptr;

    // 遍历当前节点的所有子节点，检查是否包含目标节点
    for (const auto& child : currentNode->children) {
        if (child == targetNode) {
            return currentNode; // 找到父节点
        }
        // 递归检查子节点
        auto parent = findParent(child, targetNode);
        if (parent) return parent;
    }
    return nullptr;
}
void MainWindow::onAddSibling() {
    if (!currentFamilyTree) {
        qDebug() << "No family tree selected!";
        QMessageBox::warning(this, "错误", "尚未选择家谱！");
        return;
    }

    QString targetName = ui->parentEdit->text().trimmed(); // 目标节点名称
    QString siblingName = ui->nameEdit->text().trimmed();  // 兄弟节点名称
    QString siblingDetails = ui->detailsEdit->text().trimmed(); // 兄弟节点详细信息

    if (targetName.isEmpty() || siblingName.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "目标节点和兄弟节点的名称不能为空！");
        return;
    }

    qDebug() << "Adding sibling: TargetName: " << targetName << ", SiblingName: " << siblingName;

    try {
        // 调用家谱树方法添加兄弟节点
        currentFamilyTree->addSibling(targetName, siblingName, siblingDetails);

        // 提示用户操作成功
        QMessageBox::information(this, "操作成功", "兄弟节点添加成功！");
        refreshTree(); // 刷新家谱树视图
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "操作失败", QString("兄弟节点添加失败: %1").arg(e.what()));
        qDebug() << "Exception: " << e.what();
    }
}
void MainWindow::modifySpouseDetails(const QString& memberName, const QString& spouseName, const QString& newDetails) {
    auto member = currentFamilyTree->findMember(memberName);
    if (member) {
        for (const auto& spouses : member->spouses) {
            if (spouses->name == spouseName) {
                spouses->details = newDetails;
                QMessageBox::information(this, "操作成功", QString("配偶 %1 的信息已修改为: %2").arg(spouseName, newDetails));
                return;
            }
        }
        QMessageBox::warning(this, "错误", QString("成员 %1 没有名为 %2 的配偶").arg(memberName, spouseName));
    } else {
        QMessageBox::warning(this, "错误", QString("未找到成员: %1").arg(memberName));
    }
}
void MainWindow::removeSpouse(const QString& memberName, const QString& spouseName) {
    auto member = currentFamilyTree->findMember(memberName);
    if (member) {
        for (int i = 0; i < member->spouses.size(); ++i) {
            if (member->spouses[i]->name == spouseName) {
                member->spouses.remove(i); // 从配偶列表中移除
                QMessageBox::information(this, "操作成功", QString("配偶 %1 已从成员 %2 的配偶列表中移除").arg(spouseName, memberName));
                return;
            }
        }
        QMessageBox::warning(this, "错误", QString("成员 %1 没有名为 %2 的配偶").arg(memberName, spouseName));
    } else {
        QMessageBox::warning(this, "错误", QString("未找到成员: %1").arg(memberName));
    }
}
void MainWindow::onModifySpouseDetails() {
    if (!currentFamilyTree) {
        QMessageBox::warning(this, "错误", "尚未选择家谱！");
        return;
    }

    QString memberName = ui->parentEdit->text().trimmed(); // 成员名称
    QString spouseName = ui->nameEdit->text().trimmed();   // 配偶名称
    QString newDetails = ui->detailsEdit->text().trimmed(); // 新的配偶详细信息

    if (memberName.isEmpty() || spouseName.isEmpty() || newDetails.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "成员名称、配偶名称和新的配偶信息均不能为空！");
        return;
    }

    currentFamilyTree->modifySpouseDetails(memberName, spouseName, newDetails);

    QMessageBox::information(this, "操作成功", QString("成功修改成员 %1 的配偶 %2 的信息为: %3")
                                                   .arg(memberName, spouseName, newDetails));

    refreshTree(); // 刷新树视图
}
// 导出家庭树为 CSV 文件
void MainWindow::exportFamilyTreeToCSV() {
    if (!currentFamilyTree) {
        QMessageBox::warning(this, "错误", "尚未选择家谱！");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "导出家庭树", "", "CSV 文件 (*.csv)");
    if (fileName.isEmpty()) {
        return; // 用户取消操作
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件进行写入！");
        return;
    }

    QTextStream out(&file);

    // 写入 CSV 文件头
    out << "成员名称,详细信息,配偶信息,层级" << Qt::endl;

    // 从根节点递归写入数据
    auto root = currentFamilyTree->getRoot();
    if (root) {
        writeNodeToCSV(out, root, 0);
    }

    file.close();
    QMessageBox::information(this, "导出成功", "家庭树已成功导出到：" + fileName);
}

// 递归写入节点及其子节点
void MainWindow::writeNodeToCSV(QTextStream& out, std::shared_ptr<FamilyMember> node, int level) {
    if (!node) return;

    // 写入当前节点信息
    QString spousesInfo;
    for (const auto& spouse : node->spouses) {
        spousesInfo += spouse->name + " (" + spouse->details + "); ";
    }

    // 去掉最后的分号和空格
    spousesInfo = spousesInfo.trimmed();
    if (spousesInfo.endsWith(";")) {
        spousesInfo.chop(1);
    }

    // 写入 CSV 行
    out << node->name << "," << node->details << "," << spousesInfo << "," << level << Qt::endl;

    // 递归写入子节点
    for (const auto& child : node->children) {
        writeNodeToCSV(out, child, level + 1);
    }
}
