
#pragma once
#include <Utils/IncludeRequire/GlobalRequire.h>

class FileOperationRecord final : public Orm::Tiny::Model<FileOperationRecord>,
                                  public Orm::Tiny::SoftDeletes<FileOperationRecord> {
    friend Model;

    using Model::Model;

  private:
    QString u_table{"file_operation_record"};
    QString u_primaryKey{"id"};
};

class FileShard final : public Orm::Tiny::Model<FileShard>, public Orm::Tiny::SoftDeletes<FileShard> {
    friend Model;

    using Model::Model;

  private:
    QString u_table{"file_shard"};
    QString u_primaryKey{"id"};
};