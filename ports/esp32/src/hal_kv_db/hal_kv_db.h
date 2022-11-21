#ifndef __HAL_KV_DB_H__
#define __HAL_KV_DB_H__

#if defined(__cplusplus)
extern "C" {
#endif

void hal_kv_init(void);
int hal_kv_set(const char *key, void *val, int len);
int hal_kv_get(const char *key, void *buf, int len);
int hal_kv_del(const char *key);

#if defined(__cplusplus)
}
#endif

#endif
