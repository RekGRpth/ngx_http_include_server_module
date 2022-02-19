#include <ngx_http.h>

ngx_module_t ngx_http_include_server_module;

typedef struct {
    ngx_array_t *args;
} ngx_http_include_server_main_conf_t;

typedef struct {
} ngx_http_include_server_loc_conf_t;

typedef struct {
    ngx_uint_t level;
} ngx_http_include_server_srv_conf_t;

static char *ngx_http_include_server_conf(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_include_server_main_conf_t *mcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_include_server_module);
    ngx_str_t *args, *elts = cf->args->elts;
    if (!(mcf->args = ngx_array_create(cf->pool, cf->args->nelts, sizeof(*elts)))) return NGX_CONF_ERROR;
    for (ngx_uint_t i = 0; i < cf->args->nelts; i++) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "conf elts[%i] = %V", i, &elts[i]);
        if (!(args = ngx_array_push(mcf->args))) return NGX_CONF_ERROR;
        *args = elts[i];
    }
    return NGX_CONF_OK;
}

static ngx_command_t ngx_http_include_server_commands[] = {
  { .name = ngx_string("include_server"),
    .type = NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
    .set = ngx_http_include_server_conf,
    .conf = 0,
    .offset = 0,
    .post = NULL },
    ngx_null_command
};

static void *ngx_http_include_server_create_main_conf(ngx_conf_t *cf) {
    ngx_http_include_server_main_conf_t *mcf = ngx_pcalloc(cf->pool, sizeof(*mcf));
    if (!mcf) return mcf;
    return mcf;
}

static void *ngx_http_include_server_create_srv_conf(ngx_conf_t *cf) {
    ngx_http_include_server_srv_conf_t *scf = ngx_pcalloc(cf->pool, sizeof(*scf));
    if (!scf) return scf;
    return scf;
}

static void *ngx_http_include_server_create_loc_conf(ngx_conf_t *cf) {
    ngx_http_include_server_loc_conf_t *lcf = ngx_pcalloc(cf->pool, sizeof(*lcf));
    if (!lcf) return lcf;
    if (cf->cmd_type != NGX_HTTP_SRV_CONF) return lcf;
    ngx_http_include_server_srv_conf_t *scf = ngx_http_conf_get_module_srv_conf(cf, ngx_http_include_server_module);
    if (scf->level) return lcf;
    scf->level++;
    ngx_http_include_server_main_conf_t *mcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_include_server_module);
    if (!mcf->args) return lcf;
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "create loc %i", cf->cmd_type);
    ngx_conf_t save = *cf;
    ngx_str_t *args, *elts = mcf->args->elts;
    if (!(cf->args = ngx_array_create(cf->pool, mcf->args->nelts, sizeof(*elts)))) return NULL;
    for (ngx_uint_t i = 0; i < mcf->args->nelts; i++) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "create loc elts[%i] = %V", i, &elts[i]);
        if (!(args = ngx_array_push(cf->args))) return NULL;
        *args = elts[i];
    }
    char *rv = ngx_conf_include(cf, NULL, NULL);
    *cf = save;
    if (rv != NGX_CONF_OK) return NULL;
    return lcf;
}

static ngx_http_module_t ngx_http_include_server_ctx = {
    .preconfiguration = NULL,
    .postconfiguration = NULL,
    .create_main_conf = ngx_http_include_server_create_main_conf,
    .init_main_conf = NULL,
    .create_srv_conf = ngx_http_include_server_create_srv_conf,
    .merge_srv_conf = NULL,
    .create_loc_conf = ngx_http_include_server_create_loc_conf,
    .merge_loc_conf = NULL
};

ngx_module_t ngx_http_include_server_module = {
    NGX_MODULE_V1,
    .ctx = &ngx_http_include_server_ctx,
    .commands = ngx_http_include_server_commands,
    .type = NGX_HTTP_MODULE,
    .init_master = NULL,
    .init_module = NULL,
    .init_process = NULL,
    .init_thread = NULL,
    .exit_thread = NULL,
    .exit_process = NULL,
    .exit_master = NULL,
    NGX_MODULE_V1_PADDING
};
