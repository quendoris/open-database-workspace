# Connector model

Database connectors are capability providers, not UI plugins.

A connector describes what a database engine and a particular driver implementation can actually do. ODW must not infer deep support merely from a database product name.

## Capability manifest

A connector should advertise structured capabilities such as:

```text
metadata.schemas
metadata.tables
metadata.views
metadata.materialized_views
metadata.routines
metadata.triggers
metadata.indexes
query.execute
query.cancel
query.explain
query.stream
edit.rows
transactions.explicit
transactions.savepoints
admin.users
admin.roles
admin.jobs
transfer.bulk_read
transfer.bulk_write
```

Capabilities may contain metadata rather than a single boolean. For example:

```yaml
backup_restore:
  supported: true
  implementation: external_tool
  tool: pg_dump
```

## Isolation

The default target architecture runs connectors out of process.

Reasons:

- a faulty driver must not crash the workspace;
- different connectors may have conflicting native dependencies;
- connectors can be implemented in different languages;
- resource limits and cancellation are easier to enforce;
- connector upgrades need not destabilize the core process.

## Protocol layers

The connector protocol should separate:

### Control plane

- handshake and protocol version;
- capability manifest;
- connection lifecycle;
- metadata requests;
- operation requests;
- progress;
- cancellation;
- structured errors.

A simple request/response protocol such as JSON-RPC over local IPC/stdio is a candidate for the first prototype, not a frozen decision.

### Data plane

Bulk rows and binary data should use a high-throughput representation rather than encoding every value as JSON. Candidates include Arrow IPC, shared-memory batches, or a compact ODW-native binary stream.

The final choice must be benchmarked for large result sets, cancellation, backpressure, and cross-platform behavior.

## Engine-specific surfaces

Common capabilities provide portability, but connectors may expose namespaced native capabilities such as:

```text
postgres.explain.buffers
postgres.logical_replication
sqlite.pragma
mysql.performance_schema
```

The UI may surface these explicitly when available. ODW should not force every database through a lowest-common-denominator model.

## Secrets

Connector manifests and persisted connection profiles contain references to secrets, never plaintext secrets. Secret retrieval belongs to the ODW secret-storage abstraction.

## Versioning

Connector protocol and capability schema versions must be explicit. ODW should be able to reject an incompatible provider cleanly rather than failing through undefined behavior.
