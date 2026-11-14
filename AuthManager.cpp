#include "AuthManager.h"

#include <iomanip>
#include <sstream>

namespace
{
    const unsigned long long FNV_OFFSET = 14695981039346656037ULL;
    const unsigned long long FNV_PRIME = 1099511628211ULL;
    const int HASH_ROUNDS = 10000;

    unsigned long long fnv1a(const std::string& text)
    {
        unsigned long long hash = FNV_OFFSET;
        for (char c : text)
        {
            hash ^= static_cast<unsigned char>(c);
            hash *= FNV_PRIME;
        }
        return hash;
    }

    std::string toHex(unsigned long long value)
    {
        std::stringstream out;
        out << std::hex << std::setw(16) << std::setfill('0') << value;
        return out.str();
    }
}

// Classroom prototype only; see README security note. Replace with Argon2/bcrypt
// before using real accounts or persistent credentials.
std::string AuthManager::hashPassword(const std::string& password, const std::string& salt)
{
    unsigned long long hash = fnv1a(salt + ":" + password);
    for (int i = 0; i < HASH_ROUNDS; i++)
    {
        hash = fnv1a(toHex(hash) + password);
    }
    return toHex(hash);
}

bool AuthManager::createAccount(const std::string& userId,
                                const std::string& password,
                                Role role)
{
    if (userId.empty() || password.length() < MIN_PASSWORD_LENGTH)
    {
        return false;
    }
    UserAccount account{userId, hashPassword(password, userId), role};
    return accounts.insert(userId, account);
}

bool AuthManager::login(const std::string& userId, const std::string& password) const
{
    const UserAccount* account = accounts.find(userId);
    if (account == nullptr)
    {
        return false;
    }
    return account->passwordHash == hashPassword(password, userId);
}

std::optional<Role> AuthManager::getRole(const std::string& userId) const
{
    const UserAccount* account = accounts.find(userId);
    if (account == nullptr)
    {
        return std::nullopt;
    }
    return account->role;
}

bool AuthManager::changePassword(const std::string& userId,
                                 const std::string& oldPassword,
                                 const std::string& newPassword)
{
    UserAccount* account = accounts.find(userId);
    if (account == nullptr ||
        account->passwordHash != hashPassword(oldPassword, userId) ||
        newPassword.length() < MIN_PASSWORD_LENGTH)
    {
        return false;
    }
    account->passwordHash = hashPassword(newPassword, userId);
    return true;
}

bool AuthManager::removeAccount(const std::string& userId)
{
    return accounts.erase(userId);
}

int AuthManager::getAccountCount() const
{
    return accounts.size();
}
