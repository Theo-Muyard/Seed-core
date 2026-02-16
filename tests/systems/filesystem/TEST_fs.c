#include "tools.h"
#include "seed.h"
#include "core/manager.h"
#include "systems/filesystem/system.h"
#include "systems/filesystem/_internal.h"

static void	print_vfs_tree_node(const t_Directory *dir, const char *prefix, bool is_last)
{
	size_t			i;
	size_t			total;
	char			next_prefix[1024];
	const char		*name;

	if (NULL == dir)
		return ;
	name = dir->dirname;
	if (NULL == name || '\0' == name[0])
		name = ".";
	if (NULL == prefix)
		printf("%s\n", name);
	else
		printf("%s%s%s\n", prefix, is_last ? "`-- " : "|-- ", name);
	if (NULL == prefix)
		snprintf(next_prefix, sizeof(next_prefix), "");
	else
		snprintf(next_prefix, sizeof(next_prefix), "%s%s", prefix, is_last ? "    " : "|   ");
	total = dir->subdir_count + dir->files_count;
	i = 0;
	while (i < dir->subdir_count)
	{
		print_vfs_tree_node(dir->subdir[i], next_prefix, (i + 1 == total));
		i++;
	}
	i = 0;
	while (i < dir->files_count)
	{
		printf("%s%s%s\n", next_prefix, (dir->subdir_count + i + 1 == total)
			? "`-- " : "|-- ", dir->files[i]->filename);
		i++;
	}
}

static void	print_vfs_tree(const t_Directory *root)
{
	print_section("VFS TREE");
	print_vfs_tree_node(root, NULL, true);
}

static int	assert_not_success(t_ErrorCode code, const char *message)
{
	if (ERR_SUCCESS == code)
	{
		print_error(message);
		return (1);
	}
	print_success(message);
	return (0);
}

static int	test_fs_requires_open_root(void)
{
	t_Manager		*manager;
	t_Command		cmd;
	t_CmdCreateDir	mk_dir;
	t_CmdCreateFile	mk_file;

	print_section("FS REQUIRES ROOT OPEN");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	mk_dir.path = "a";
	cmd.id = CMD_FS_CREATE_DIR;
	cmd.payload = &mk_dir;
	if (assert_error_code(
		manager_exec(manager, &cmd),
		ERR_FS_CONTEXT_NOT_INITIALIZED,
		"Create dir rejected without root"
	))
		return (manager_clean(manager), 1);
	mk_file.path = "a/test.txt";
	cmd.id = CMD_FS_CREATE_FILE;
	cmd.payload = &mk_file;
	if (assert_error_code(
		manager_exec(manager, &cmd),
		ERR_FS_CONTEXT_NOT_INITIALIZED,
		"Create file rejected without root"
	))
		return (manager_clean(manager), 1);
	manager_clean(manager);
	return (0);
}

static int	test_root_open_validation(void)
{
	t_Manager		*manager;
	t_Command		cmd;
	t_CmdOpenRoot	open_payload;
	t_ErrorCode		err;
	char			*tmp_root;
	char			*tmp_file;
	FILE			*f;

	print_section("ROOT OPEN VALIDATION");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	tmp_root = test_tmpdir_create("/tmp/seed_fs_root_validation");
	if (NULL == tmp_root)
		return (manager_clean(manager), print_error("Failed to create tmp root"), 1);
	open_payload.path = "/tmp/seed_fs_path_that_does_not_exist";
	cmd.id = CMD_FS_OPEN_ROOT;
	cmd.payload = &open_payload;
	err = manager_exec(manager, &cmd);
	if (assert_not_success(err, "Open root rejected on missing path"))
		goto fail;

	tmp_file = malloc(strlen(tmp_root) + sizeof("/not_a_dir.txt"));
	if (NULL == tmp_file)
		goto fail;
	strcpy(tmp_file, tmp_root);
	strcat(tmp_file, "/not_a_dir.txt");
	f = fopen(tmp_file, "w");
	if (NULL == f)
		goto fail_with_file;
	fclose(f);
	open_payload.path = tmp_file;
	err = manager_exec(manager, &cmd);
	if (assert_error_code(err, ERR_DIR_NOT_FOUND, "Open root rejected on regular file"))
		goto fail_with_file;

	open_payload.path = tmp_root;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Open valid root"))
		goto fail_with_file;
	if (assert_error_code((manager_exec(manager, &(t_Command){CMD_FS_CLOSE_ROOT, NULL})), ERR_SUCCESS, "Close root"))
		goto fail_with_file;
	if (assert_error_code((manager_exec(manager, &(t_Command){CMD_FS_CLOSE_ROOT, NULL})), ERR_FS_CONTEXT_NOT_INITIALIZED, "Close root rejected when already closed"))
		goto fail_with_file;
	free(tmp_file);
	if (false == test_tmpdir_remove(tmp_root))
		goto fail;
	free(tmp_root);
	manager_clean(manager);
	return (0);

fail_with_file:
	free(tmp_file);
fail:
	test_tmpdir_remove(tmp_root);
	free(tmp_root);
	manager_clean(manager);
	return (1);
}

static int	test_root_populates_vfs(void)
{
	t_Manager		*manager;
	t_Command		cmd;
	t_CmdOpenRoot	open_payload;
	char			*tmp_root;
	char			*path_pre;
	char			*path_sub;
	char			*path_file;
	FILE			*f;
	t_Directory		*found_dir;
	t_File			*found_file;

	print_section("ROOT POPULATES VFS");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	tmp_root = test_tmpdir_create("/tmp/seed_fs_root_populate");
	if (NULL == tmp_root)
		return (manager_clean(manager), print_error("Failed to create tmp root"), 1);
	path_pre = malloc(strlen(tmp_root) + sizeof("/pre"));
	path_sub = malloc(strlen(tmp_root) + sizeof("/pre/sub"));
	path_file = malloc(strlen(tmp_root) + sizeof("/pre/sub/file.txt"));
	if (!path_pre || !path_sub || !path_file)
		goto fail;
	strcpy(path_pre, tmp_root);
	strcat(path_pre, "/pre");
	strcpy(path_sub, tmp_root);
	strcat(path_sub, "/pre/sub");
	strcpy(path_file, tmp_root);
	strcat(path_file, "/pre/sub/file.txt");
	if (-1 == mkdir(path_pre, 0755) || -1 == mkdir(path_sub, 0755))
		goto fail;
	f = fopen(path_file, "w");
	if (NULL == f)
		goto fail;
	fprintf(f, "existing\n");
	fclose(f);

	open_payload.path = tmp_root;
	cmd.id = CMD_FS_OPEN_ROOT;
	cmd.payload = &open_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Open root with existing content"))
		goto fail;
	found_dir = directory_resolve(manager->fs_ctx->root, "pre/sub");
	if (NULL == found_dir)
		goto fail;
	print_success("Existing directories loaded in VFS");
	found_file = file_resolve(manager->fs_ctx->root, "pre/sub/file.txt");
	if (NULL == found_file)
		goto fail;
	print_success("Existing file loaded in VFS");
	print_vfs_tree(manager->fs_ctx->root);
	if (assert_error_code((manager_exec(manager, &(t_Command){CMD_FS_CLOSE_ROOT, NULL})), ERR_SUCCESS, "Close root"))
		goto fail;

	free(path_pre);
	free(path_sub);
	free(path_file);
	test_tmpdir_remove(tmp_root);
	free(tmp_root);
	manager_clean(manager);
	return (0);

fail:
	print_error("VFS population test failed");
	free(path_pre);
	free(path_sub);
	free(path_file);
	test_tmpdir_remove(tmp_root);
	free(tmp_root);
	manager_clean(manager);
	return (1);
}

static int	test_directory_and_file_lifecycle(void)
{
	t_Manager		*manager;
	t_Command		cmd;
	t_CmdOpenRoot	open_root;
	t_CmdCreateDir	mk_dir;
	t_CmdCreateFile	mk_file;
	t_CmdWriteFile	wr;
	t_CmdReadFile		rd;
	t_CmdMoveFile	mv_file;
	t_CmdMoveDir	mv_dir;
	t_CmdDeleteFile	rm_file;
	t_CmdDeleteDir	rm_dir;
	char			*tmp_root;

	print_section("DIRECTORY & FILE LIFECYCLE");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	tmp_root = test_tmpdir_create("/tmp/seed_fs_lifecycle");
	if (NULL == tmp_root)
		return (manager_clean(manager), print_error("Failed to create tmp root"), 1);

	open_root.path = tmp_root;
	cmd.id = CMD_FS_OPEN_ROOT;
	cmd.payload = &open_root;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Open root"))
		goto fail;
	mk_dir.path = "a";
	cmd.id = CMD_FS_CREATE_DIR;
	cmd.payload = &mk_dir;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Create dir a"))
		goto fail;
	mk_dir.path = "a/b";
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Create dir a/b"))
		goto fail;
	mk_file.path = "a/b/file.txt";
	cmd.id = CMD_FS_CREATE_FILE;
	cmd.payload = &mk_file;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Create file"))
		goto fail;

	wr.path = "a/b/file.txt";
	wr.data = "hello seed\n";
	cmd.id = CMD_FS_WRITE_FILE;
	cmd.payload = &wr;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Write file"))
		goto fail;

	rd.path = "a/b/file.txt";
	rd.out_data = NULL;
	rd.out_len = 0;
	cmd.id = CMD_FS_READ_FILE;
	cmd.payload = &rd;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Read file"))
		goto fail;
	if (0 != strcmp(rd.out_data, "hello seed\n"))
		return (free(rd.out_data), print_error("Read content mismatch"), 1);
	print_success("Read content is correct");
	free(rd.out_data);

	mv_file.old_path = "a/b/file.txt";
	mv_file.new_path = "a/b/file2.txt";
	cmd.id = CMD_FS_MOVE_FILE;
	cmd.payload = &mv_file;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Move file"))
		goto fail;

	mv_dir.old_path = "a/b";
	mv_dir.new_path = "a/c";
	cmd.id = CMD_FS_MOVE_DIR;
	cmd.payload = &mv_dir;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Move dir"))
		goto fail;

	rm_file.path = "a/c/file2.txt";
	cmd.id = CMD_FS_DELETE_FILE;
	cmd.payload = &rm_file;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Delete moved file"))
		goto fail;
	rm_dir.path = "a/c";
	cmd.id = CMD_FS_DELETE_DIR;
	cmd.payload = &rm_dir;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Delete moved dir"))
		goto fail;
	rm_dir.path = "a";
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Delete parent dir"))
		goto fail;
	if (assert_error_code((manager_exec(manager, &(t_Command){CMD_FS_CLOSE_ROOT, NULL})), ERR_SUCCESS, "Close root"))
		goto fail;

	test_tmpdir_remove(tmp_root);
	free(tmp_root);
	manager_clean(manager);
	return (0);

fail:
	print_error("Lifecycle failed");
	manager_exec(manager, &(t_Command){CMD_FS_CLOSE_ROOT, NULL});
	test_tmpdir_remove(tmp_root);
	free(tmp_root);
	manager_clean(manager);
	return (1);
}

static int	test_filesystem_error_cases(void)
{
	t_Manager		*manager;
	t_Command		cmd;
	t_CmdOpenRoot	open_root;
	t_CmdCreateDir	mk_dir;
	t_CmdDeleteDir	rm_dir;
	t_CmdDeleteFile	rm_file;
	t_CmdMoveFile	mv_file;
	t_CmdReadFile	rd_file;
	t_CmdWriteFile	wr_file;
	char			*tmp_root;

	print_section("FILESYSTEM ERROR CASES");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	tmp_root = test_tmpdir_create("/tmp/seed_fs_errors");
	if (NULL == tmp_root)
		return (manager_clean(manager), print_error("Failed to create tmp root"), 1);
	open_root.path = tmp_root;
	cmd.id = CMD_FS_OPEN_ROOT;
	cmd.payload = &open_root;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Open root"))
		goto fail;

	mk_dir.path = "same";
	cmd.id = CMD_FS_CREATE_DIR;
	cmd.payload = &mk_dir;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Create dir for duplicate test"))
		goto fail;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_DIR_EXIST, "Duplicate dir create rejected"))
		goto fail;

	rm_dir.path = "missing_dir";
	cmd.id = CMD_FS_DELETE_DIR;
	cmd.payload = &rm_dir;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_DIR_NOT_FOUND, "Delete missing dir rejected"))
		goto fail;

	rm_file.path = "missing.txt";
	cmd.id = CMD_FS_DELETE_FILE;
	cmd.payload = &rm_file;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_FILE_NOT_FOUND, "Delete missing file rejected"))
		goto fail;

	mv_file.old_path = "missing.txt";
	mv_file.new_path = "renamed.txt";
	cmd.id = CMD_FS_MOVE_FILE;
	cmd.payload = &mv_file;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_FILE_NOT_FOUND, "Move missing file rejected"))
		goto fail;

	rd_file.path = "missing.txt";
	rd_file.out_data = NULL;
	rd_file.out_len = 0;
	cmd.id = CMD_FS_READ_FILE;
	cmd.payload = &rd_file;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_OPERATION_FAILED, "Read missing file rejected"))
		goto fail;

	wr_file.path = "missing_parent/x.txt";
	wr_file.data = "x";
	cmd.id = CMD_FS_WRITE_FILE;
	cmd.payload = &wr_file;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_OPERATION_FAILED, "Write into missing parent rejected"))
		goto fail;

	manager_exec(manager, &(t_Command){CMD_FS_CLOSE_ROOT, NULL});
	test_tmpdir_remove(tmp_root);
	free(tmp_root);
	manager_clean(manager);
	return (0);

fail:
	manager_exec(manager, &(t_Command){CMD_FS_CLOSE_ROOT, NULL});
	test_tmpdir_remove(tmp_root);
	free(tmp_root);
	manager_clean(manager);
	return (1);
}

int	main(void)
{
	int	status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║      FILESYSTEM TEST SUITE         ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);
	status = 0;
	status |= test_fs_requires_open_root();
	status |= test_root_open_validation();
	status |= test_root_populates_vfs();
	status |= test_directory_and_file_lifecycle();
	status |= test_filesystem_error_cases();
	print_status(status);
	return (status);
}
