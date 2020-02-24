#include <common/cdef.h>
#include <ke/spin_lock.h>

static inline uint32
_spin_lock_get_ticket(uint32 val)
{
    return val & 0xFFFFu;
}

static inline uint32
_spin_lock_get_owner(uint32 val)
{
    return val >> 16u;
}

void
ke_spin_lock_init(struct ke_spin_lock *lock)
{
    atomic_store(&lock->val, 0);
}

void
ke_spin_lock_acq(struct ke_spin_lock *lock)
{
    uint32 val;

    do {
        val = atomic_load(&lock->val);
    } while (!atomic_compare_exchange_weak(&lock->val, &val, val + (1u << 16u)));

    // val now contains cur ticket and target ticket
    while (_spin_lock_get_ticket(val) != _spin_lock_get_owner(val)) {
        val = atomic_load(&lock->val);
    }

    // owner = ticket, we've acquired the lock
}

void
ke_spin_lock_rel(struct ke_spin_lock *lock)
{
    // increment ticket
    atomic_fetch_add(&lock->val, 1);
}

int
ke_spin_lock_try_acq(struct ke_spin_lock *lock)
{
    uint32 val;

    val = atomic_load(&lock->val);

    if ((_spin_lock_get_owner(val) == _spin_lock_get_ticket(val)) &&
        atomic_compare_exchange_weak(&lock->val, &val, val + (1u << 16u))) {
        return 1;
    }

    return 0;
}

