#include "kismet/ai/fuzzy/fuzzy_set.h"

namespace kismet
{
namespace fuzzy
{

fuzzy_set::fuzzy_set(float mean)
    : m_mean_max{ mean }
{
}

fuzzy_set::~fuzzy_set() = default;

} // namespace fuzzy
} // namespace kismet
