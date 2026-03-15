#pragma once
#include <vector>
#include <cstdint>
#include <limits>

template<typename T>
class SlotMap
{
public:

    struct Key
    {
        uint32_t index;
        uint32_t generation;

        bool operator==(const Key& other) const
        {
            return index == other.index && generation == other.generation;
        }
    };

private:

    static constexpr uint32_t invalid = std::numeric_limits<uint32_t>::max();

    struct Slot
    {
        uint32_t generation = 0;
        uint32_t denseIndex = invalid;
        uint32_t nextFree = invalid;
        bool occupied = false;
    };

    std::vector<T> values;
    std::vector<Slot> slots;
    std::vector<uint32_t> denseToSlot;

    uint32_t freeHead = invalid;

public:

    Key insert(const T& value)
    {
        uint32_t slotIndex;

        if (freeHead != invalid)
        {
            slotIndex = freeHead;
            freeHead = slots[slotIndex].nextFree;
        }
        else
        {
            slotIndex = (uint32_t)slots.size();
            slots.push_back({});
        }

        Slot& slot = slots[slotIndex];
        slot.occupied = true;

        uint32_t denseIndex = (uint32_t)values.size();

        values.push_back(value);
        denseToSlot.push_back(slotIndex);

        slot.denseIndex = denseIndex;

        return { slotIndex, slot.generation };
    }

    bool contains(Key key) const
    {
        if (key.index >= slots.size())
            return false;

        const Slot& s = slots[key.index];

        return s.occupied && s.generation == key.generation;
    }

    T* get(Key key)
    {
        if (!contains(key))
            return nullptr;

        return &values[slots[key.index].denseIndex];
    }

    const T* get(Key key) const
    {
        if (!contains(key))
            return nullptr;

        return &values[slots[key.index].denseIndex];
    }

    void erase(Key key)
    {
        if (!contains(key))
            return;

        uint32_t slotIndex = key.index;
        Slot& slot = slots[slotIndex];

        uint32_t denseIndex = slot.denseIndex;
        uint32_t lastDense = (uint32_t)values.size() - 1;

        if (denseIndex != lastDense)
        {
            values[denseIndex] = std::move(values[lastDense]);

            uint32_t movedSlot = denseToSlot[lastDense];

            denseToSlot[denseIndex] = movedSlot;
            slots[movedSlot].denseIndex = denseIndex;
        }

        values.pop_back();
        denseToSlot.pop_back();

        slot.occupied = false;
        slot.generation++;
        slot.denseIndex = invalid;

        slot.nextFree = freeHead;
        freeHead = slotIndex;
    }

    std::vector<T>& data() { return values; }
    const std::vector<T>& data() const { return values; }

    size_t size() const { return values.size(); }
};