/**
 *    Copyright (C) 2016 MongoDB Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the GNU Affero General Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kSharding

#include "mongo/platform/basic.h"

#include "mongo/db/s/sharding_egress_metadata_hook_for_mongod.h"

#include "mongo/base/status.h"
#include "mongo/db/repl/replication_coordinator.h"
#include "mongo/db/server_options.h"
#include "mongo/s/grid.h"

namespace mongo {
namespace rpc {

void ShardingEgressMetadataHookForMongod::_saveGLEStats(const BSONObj& metadata,
                                                        StringData hostString) {}

repl::OpTime ShardingEgressMetadataHookForMongod::_getConfigServerOpTime() {
    if (serverGlobalParams.clusterRole == ClusterRole::ConfigServer) {
        return repl::ReplicationCoordinator::get(_serviceContext)
            ->getCurrentCommittedSnapshotOpTime();
    } else {
        // TODO uncomment as part of SERVER-22663
        // invariant(serverGlobalParams.clusterRole == ClusterRole::ShardServer);
        return grid.configOpTime();
    }
}

Status ShardingEgressMetadataHookForMongod::_advanceConfigOptimeFromShard(
    ShardId shardId, const BSONObj& metadataObj) {
    if (serverGlobalParams.clusterRole == ClusterRole::ConfigServer) {
        return Status::OK();
    }
    return ShardingEgressMetadataHook::_advanceConfigOptimeFromShard(shardId, metadataObj);
}

}  // namespace rpc
}  // namespace mongo
