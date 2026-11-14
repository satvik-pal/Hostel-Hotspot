#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <optional>
#include <string>
#include "HashMap.h"

enum class Role
{
    Student,
    Admin
};

struct UserAccount
{
    std::string userId;
    std::string passwordHash;
    Role role;
};

class AuthManager
{
private:
    HashMap<UserAccount> accounts;

public:
    static constexpr int MIN_PASSWORD_LENGTH = 6;
    static std::string hashPassword(const std::string& password, const std::string& salt);

    bool createAccount(const std::string& userId, const std::string& password, Role role);
    bool login(const std::string& userId, const std::string& password) const;
    std::optional<Role> getRole(const std::string& userId) const;
    bool changePassword(const std::string& userId,
                        const std::string& oldPassword,
                        const std::string& newPassword);
    bool removeAccount(const std::string& userId);
    int getAccountCount() const;
};

#endif
