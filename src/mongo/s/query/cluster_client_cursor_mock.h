/**
 *    Copyright (C) 2015 MongoDB Inc.
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

#pragma once

#include <queue>

#include <boost/optional.hpp>

#include "mongo/db/logical_session_id.h"
#include "mongo/s/query/cluster_client_cursor.h"
#include "mongo/stdx/functional.h"

namespace mongo {

class ClusterClientCursorMock final : public ClusterClientCursor {
    MONGO_DISALLOW_COPYING(ClusterClientCursorMock);

public:
    ClusterClientCursorMock(boost::optional<LogicalSessionId> lsid,
                            stdx::function<void(void)> killCallback = stdx::function<void(void)>());

    ~ClusterClientCursorMock();

    StatusWith<ClusterQueryResult> next(RouterExecStage::ExecContext) final;

    void kill(OperationContext* opCtx) final;

    void reattachToOperationContext(OperationContext* opCtx) final {
        _opCtx = opCtx;
    }

    void detachFromOperationContext() final {
        _opCtx = nullptr;
    }

    OperationContext* getCurrentOperationContext() const final {
        return _opCtx;
    }

    bool isTailable() const final;

    bool isTailableAndAwaitData() const final;

    long long getNumReturnedSoFar() const final;

    void queueResult(const ClusterQueryResult& result) final;

    Status setAwaitDataTimeout(Milliseconds awaitDataTimeout) final;

    boost::optional<LogicalSessionId> getLsid() const final;

    boost::optional<ReadPreferenceSetting> getReadPreference() const final;

    /**
     * Returns true unless marked as having non-exhausted remote cursors via
     * markRemotesNotExhausted().
     */
    bool remotesExhausted() final;

    void markRemotesNotExhausted();

    /**
     * Queues an error response.
     */
    void queueError(Status status);

private:
    bool _killed = false;
    bool _exhausted = false;
    std::queue<StatusWith<ClusterQueryResult>> _resultsQueue;
    stdx::function<void(void)> _killCallback;

    // Number of returned documents.
    long long _numReturnedSoFar = 0;

    bool _remotesExhausted = true;

    boost::optional<LogicalSessionId> _lsid;

    OperationContext* _opCtx = nullptr;
};

}  // namespace mongo
