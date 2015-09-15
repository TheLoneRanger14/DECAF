{
  .name         = "do_hookapitests",
  .args_type    = _QEMU_MON_KEY_VALUE("procname","s"),
  ._QEMU_MON_HANDLER_CMD = do_hookapitests,
  .params       = "[procname]",
  .help	        = "Start tracing process with name [process_name]"
},
