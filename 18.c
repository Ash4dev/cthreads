/*
 * File: 18.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-08
 * Description: Binary semaphore
 * NOTE: keep it working & simple
 */

// NOTE: Mutexes are meant to represent ownership by one thread: lock & unlock
// NOTE: Binary semaphores are for signaling, NOT resource counting / ownership

// NOTE: locking thread must also unlock that mutex -> else undefined behavior
