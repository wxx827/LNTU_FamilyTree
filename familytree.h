#ifndef FAMILYTREE_H
#define FAMILYTREE_H

#include <QString>
#include <QList>
#include <memory>

struct FamilyMember {
    QString name;
    QString details;
    std::shared_ptr<FamilyMember> spouse;
    QList<std::shared_ptr<FamilyMember>> children;

    FamilyMember(const QString& name, const QString& details)
        : name(name), details(details), spouse(nullptr) {}
};

class FamilyTree {
public:
    FamilyTree();
    explicit FamilyTree(const QString& name);

    void addMember(const QString& parentName, const QString& name, const QString& details);
    void addSpouse(const QString& memberName, const QString& spouseName, const QString& spouseDetails);
    void addSibling(const QString& targetName, const QString& siblingName, const QString& siblingDetails); // 添加兄弟节点
    void modifyMember(const QString& name, const QString& newDetails);
    std::shared_ptr<FamilyMember> findMember(const QString& name);

private:
    std::shared_ptr<FamilyMember> findRecursive(const QString& name, std::shared_ptr<FamilyMember> node);
    std::shared_ptr<FamilyMember> findParent(std::shared_ptr<FamilyMember> currentNode, std::shared_ptr<FamilyMember> targetNode); // 查找父节点

    QString treeName;
    std::shared_ptr<FamilyMember> root;

public:
    std::shared_ptr<FamilyMember> getRoot() const { return root; }
};

#endif // FAMILYTREE_H
